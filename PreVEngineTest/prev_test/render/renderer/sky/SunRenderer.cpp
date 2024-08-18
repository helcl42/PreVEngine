#include "SunRenderer.h"
#include "SkyEvents.h"

#include "../../../common/AssetManager.h"
#include "../../../component/sky/ISunComponent.h"

#include <prev/event/EventChannel.h>
#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::sky {
SunRenderer::SunRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
{
}

void SunRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/sun_occlusion_vert.spv") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/sun_occlusion_frag.spv") }
        })
        .AddVertexInputAttributeDescriptions({
            prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
        })
        .AddVertexInputBindingDescriptions({
            prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        })
        .AddDescriptorSets({
            { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT }
        })
        .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Sun Shader created\n");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(true)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(VK_POLYGON_MODE_FILL)
        .SetCullingMode(VK_CULL_MODE_BACK_BIT)
        .Build();
    // clang-format on

    LOGI("Sun Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsVS>>(m_allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    for (uint32_t i = 0; i < QueryPoolCount; ++i) {
        VkQueryPoolCreateInfo queryPoolInfo = { VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO };
        queryPoolInfo.queryType = VK_QUERY_TYPE_OCCLUSION;
        queryPoolInfo.queryCount = 1;
        VKERRCHECK(vkCreateQueryPool(m_device, &queryPoolInfo, nullptr, &m_queryPools[i]));
    }

    m_frameIndex = 0;
    m_queryPoolIndex = 0;
    m_passedSamples = 0;
}

void SunRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
    if (m_frameIndex >= QueryPoolCount) {
#if defined(TARGET_PLATFORM_ANDROID)
        VkQueryResultFlags queryResultFlags{ VK_QUERY_RESULT_PARTIAL_BIT };
#else
        VkQueryResultFlags queryResultFlags{ VK_QUERY_RESULT_STATUS_COMPLETE_KHR };
#endif
        const auto result{ vkGetQueryPoolResults(m_device, m_queryPools[(m_queryPoolIndex + QueryPoolCount - 1) % QueryPoolCount], 0, 1, sizeof(m_passedSamples), &m_passedSamples, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | queryResultFlags) };

#if defined(TARGET_PLATFORM_IOS) || defined(TARGET_PLATFORM_MACOS)
        const float ratio{ m_passedSamples > 0.0f ? 0.5f : 0.0f };
#else
        const float ratio{ glm::clamp((static_cast<float>(m_passedSamples) / static_cast<float>(m_maxNumberOfSamples * m_renderPass.GetSamplesCount())) * 0.5f, 0.0f, 1.0f) };
#endif
        prev::event::EventChannel::Post(prev_test::render::renderer::sky::SunVisibilityEvent{ ratio });
    }

    vkCmdResetQueryPool(renderContext.commandBuffer, m_queryPools[m_queryPoolIndex], 0, 1);
}

void SunRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void SunRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_SUN_RENDER_COMPONENT })) {
        const auto sunComponent{ prev::scene::component::ComponentRepository<prev_test::component::sky::ISunComponent>::Instance().Get(node->GetId()) };

        const float aspectRatio{ static_cast<float>(renderContext.rect.extent.width - renderContext.rect.offset.x) / static_cast<float>(renderContext.rect.extent.height - renderContext.rect.offset.y) };
        const float xScale{ sunComponent->GetFlare()->GetScale() };
        const float yScale{ xScale * aspectRatio };

        m_maxNumberOfSamples = static_cast<uint64_t>(xScale * static_cast<float>(renderContext.rect.extent.width - renderContext.rect.offset.x) * yScale * static_cast<float>(renderContext.rect.extent.height - renderContext.rect.offset.y));

        vkCmdBeginQuery(renderContext.commandBuffer, m_queryPools[m_queryPoolIndex], 0, 0);

        auto uboVS = m_uniformsPoolVS->GetNext();
        UniformsVS uniformsVS{};
        uniformsVS.translation = glm::vec4(sunComponent->GetFlare()->GetScreenSpacePosition(), MAX_DEPTH, 1.0f);
        uniformsVS.scale = glm::vec4(xScale, yScale, 0.0f, 0.0f);
        uboVS->Update(&uniformsVS);

        m_shader->Bind("uboVS", *uboVS);

        const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *sunComponent->GetModel()->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *sunComponent->GetModel()->GetIndexBuffer(), 0, sunComponent->GetModel()->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, sunComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);

        vkCmdEndQuery(renderContext.commandBuffer, m_queryPools[m_queryPoolIndex], 0);
    }
}

void SunRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void SunRenderer::AfterRender(const NormalRenderContext& renderContext)
{
    m_queryPoolIndex = (m_queryPoolIndex + 1) % QueryPoolCount;
    ++m_frameIndex;
}

void SunRenderer::ShutDown()
{
    for (uint32_t i = 0; i < QueryPoolCount; ++i) {
        vkDestroyQueryPool(m_device, m_queryPools[i], nullptr);
    }

    m_pipeline = nullptr;
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::sky
