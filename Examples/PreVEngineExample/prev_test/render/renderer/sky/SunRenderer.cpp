#include "SunRenderer.h"
#include "SkyEvents.h"

#include "../../../Tags.h"
#include "../../../common/AssetManager.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/sky/ISunComponent.h"

#include <prev/event/EventChannel.h>
#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/query/QueryPoolBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::sky {
SunRenderer::SunRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
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

    LOGI("Sun Shader created");

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

    LOGI("Sun Pipeline created");

    m_uniformsPoolVS = prev::render::buffer::BufferPoolBuilder{ m_allocator }
                           .SetMemoryType(prev::core::memory::MemoryType::HOST_MAPPED)
                           .SetUsageFlags(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
                           .SetCount(m_descriptorCount)
                           .SetStride(sizeof(UniformsVS))
                           .SetAlignment(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment)
                           .Build();

    LOGI("Sun Uniforms Pools created");

    m_queryPool = prev::render::query::QueryPoolBuilder{ m_device }
                      .SetQueryType(VK_QUERY_TYPE_OCCLUSION)
                      .SetPoolCount(QueryPoolCount)
                      .SetQueryCount(MAX_VIEW_COUNT)
                      .Build();
    m_passedSamples = 0;

    LOGI("Sun Query Pool created");
}

void SunRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
#if defined(TARGET_PLATFORM_ANDROID)
    VkQueryResultFlags queryResultFlags{ VK_QUERY_RESULT_PARTIAL_BIT };
#else
    VkQueryResultFlags queryResultFlags{ VK_QUERY_RESULT_STATUS_COMPLETE_KHR };
#endif
    if (m_queryPool->GetQueryResult(0, queryResultFlags | VK_QUERY_RESULT_64_BIT, m_passedSamples)) {
#if defined(TARGET_PLATFORM_IOS) || defined(TARGET_PLATFORM_MACOS)
        const float ratio{ m_passedSamples > 0.0f ? 0.5f : 0.0f };
#else
        const float ratio{ glm::clamp((static_cast<float>(m_passedSamples) / static_cast<float>(m_maxNumberOfSamples * m_renderPass.GetSamplesCount())) * 0.5f, 0.0f, 1.0f) };
#endif
        prev::event::EventChannel::Post(prev_test::render::renderer::sky::SunVisibilityEvent{ ratio });
    }
    m_queryPool->Reset(renderContext.commandBuffer);
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
    if (!node->GetTags().HasAll({ TAG_SUN_RENDER_COMPONENT })) {
        return;
    }

    const auto sunComponent{ prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::sky::ISunComponent>(node) };
    const auto lightComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>(m_scene.GetRootNode(), { TAG_MAIN_LIGHT }) };

    const float aspectRatio{
        static_cast<float>(renderContext.rect.extent.width - renderContext.rect.offset.x) / static_cast<float>(renderContext.rect.extent.height - renderContext.rect.offset.y)
    };
    const float xScale{ sunComponent->GetFlare().GetScale() };
    const float yScale{ xScale * aspectRatio };

    m_maxNumberOfSamples = static_cast<uint64_t>(xScale * static_cast<float>(renderContext.rect.extent.width - renderContext.rect.offset.x) * yScale * static_cast<float>(renderContext.rect.extent.height - renderContext.rect.offset.y));
    m_queryPool->BeginQuery(0, renderContext.commandBuffer);

    m_uniformsPoolVS->MoveToNext();

    auto& uboVS = m_uniformsPoolVS->GetCurrent();
    UniformsVS uniformsVS{};
    for (uint32_t viewIndex = 0; viewIndex < renderContext.cameraCount; ++viewIndex) {
        const auto sunPosition{ sunComponent->ComputeFlarePosition(renderContext.projectionMatrices[viewIndex], renderContext.viewMatrices[viewIndex], renderContext.cameraPositions[viewIndex], lightComponent->GetPosition()) };
        uniformsVS.translations[viewIndex] = glm::vec4(sunPosition, MAX_DEPTH, 1.0f);
    }
    uniformsVS.scale = glm::vec4(xScale, yScale, 0.0f, 0.0f);
    uboVS.Write(uniformsVS);

    m_shader->Bind("uboVS", uboVS);

    const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
    const VkBuffer vertexBuffers[] = { *sunComponent->GetModel()->GetVertexBuffer() };
    const VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(renderContext.commandBuffer, *sunComponent->GetModel()->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDrawIndexed(renderContext.commandBuffer, sunComponent->GetModel()->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);

    m_queryPool->EndQuery(0, renderContext.commandBuffer);
}

void SunRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void SunRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void SunRenderer::ShutDown()
{
    m_queryPool = {};

    m_uniformsPoolVS = {};

    m_pipeline = {};
    m_shader = {};
}
} // namespace prev_test::render::renderer::sky
