#include "SkyBoxRenderer.h"

#include "../../../common/AssetManager.h"
#include "../../../component/sky//ISkyBoxComponent.h"
#include "../../../component/sky/SkyCommon.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::sky {
SkyBoxRenderer::SkyBoxRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
{
}

void SkyBoxRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/skybox_vert.spv") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/skybox_frag.spv") }
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
            { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },
            { "uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "cubeMap1", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
        })
        .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Skybox Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(false)
        .SetDepthWriteEnabled(false)
        .SetBlendingModeEnabled(false)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(VK_POLYGON_MODE_FILL)
        .SetCullingMode(VK_CULL_MODE_BACK_BIT)
        .Build();
    // clang-format on

    LOGI("Skybox Pipeline created");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsVS>>(m_allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsFS>>(m_allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
}

void SkyBoxRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void SkyBoxRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void SkyBoxRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_SKYBOX_RENDER_COMPONENT, TAG_TRANSFORM_COMPONENT })) {
        const auto transformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(node->GetId());
        const auto skyBoxComponent = prev::scene::component::ComponentRepository<prev_test::component::sky::ISkyBoxComponent>::Instance().Get(node->GetId());

        auto uboVS = m_uniformsPoolVS->GetNext();

        UniformsVS uniformsVS{};
        uniformsVS.modelMatrix = transformComponent->GetWorldTransformScaled();
        for(uint32_t i = 0; i < renderContext.cameraCount; ++i) {
            uniformsVS.viewMatrices[i] = renderContext.viewMatrices[i];
            uniformsVS.projectionMatrices[i] = renderContext.projectionMatrices[i];
        }
        uboVS->Update(&uniformsVS);

        auto uboFS = m_uniformsPoolFS->GetNext();

        UniformsFS uniformsFS{};
        uniformsFS.fogColor = prev_test::component::sky::FOG_COLOR;
        uniformsFS.lowerLimit = glm::vec4(0.0f);
        uniformsFS.upperLimit = glm::vec4(0.03f);
        uboFS->Update(&uniformsFS);

        m_shader->Bind("cubeMap1", *skyBoxComponent->GetMaterial()->GetImageBuffer(), *skyBoxComponent->GetMaterial()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_shader->Bind("uboVS", *uboVS);
        m_shader->Bind("uboFS", *uboFS);

        const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *skyBoxComponent->GetModel()->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *skyBoxComponent->GetModel()->GetIndexBuffer(), 0, skyBoxComponent->GetModel()->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, skyBoxComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }
}

void SkyBoxRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void SkyBoxRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void SkyBoxRenderer::ShutDown()
{
    m_pipeline = nullptr;
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::sky
