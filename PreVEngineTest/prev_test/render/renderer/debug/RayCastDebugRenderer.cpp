#include "RayCastDebugRenderer.h"

#ifdef RENDER_RAYCASTS

#include "../../../Tags.h"
#include "../../../common/AssetManager.h"
#include "../../../component/ray_casting/IRayCasterComponent.h"

#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::debug {
RayCastDebugRenderer::RayCastDebugRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void RayCastDebugRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/debug/raycast_debug_vert.spv") },
            { VK_SHADER_STAGE_GEOMETRY_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/debug/raycast_debug_geom.spv") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/debug/raycast_debug_frag.spv") }
        })
        .AddVertexInputAttributeDescriptions({
            prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
        })
        .AddVertexInputBindingDescriptions({
            prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        })
        .AddDescriptorSets({
            { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },
            { "uboGS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_GEOMETRY_BIT },
            { "uboFS", 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
        })
	    .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("RayCast Debug Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(false)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(VK_POLYGON_MODE_FILL)
        .SetCullingMode(VK_CULL_MODE_NONE)
        .Build();
    // clang-format on

    LOGI("RayCast Debug Pipeline created");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformRingBuffer<UniformsVS>>(m_allocator);
    m_uniformsPoolVS->UpdateCapacity(m_descriptorCount, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolGS = std::make_unique<prev::render::buffer::UniformRingBuffer<UniformsGS>>(m_allocator);
    m_uniformsPoolGS->UpdateCapacity(m_descriptorCount, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::render::buffer::UniformRingBuffer<UniformsFS>>(m_allocator);
    m_uniformsPoolFS->UpdateCapacity(m_descriptorCount, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
}

void RayCastDebugRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void RayCastDebugRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void RayCastDebugRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_RAYCASTER_COMPONENT })) {
        return;
    }

    const auto rayCastingComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::ray_casting::IRayCasterComponent>(node);

    auto uboVS = m_uniformsPoolVS->GetNext();

    UniformsVS uniformsVS{};
    uniformsVS.color = glm::vec3(1.0, 0.0, 0.0);

    uboVS->Data(uniformsVS);

    auto uboGS = m_uniformsPoolGS->GetNext();

    UniformsGS uniformsGS{};
    uniformsGS.modelMatrix = glm::mat4(1.0f);
    for (uint32_t i = 0; i < renderContext.cameraCount; ++i) {
        uniformsGS.viewMatrices[i] = renderContext.viewMatrices[i];
        uniformsGS.projectionMatrices[i] = renderContext.projectionMatrices[i];
    }

    uboGS->Data(uniformsGS);

    auto uboFS = m_uniformsPoolFS->GetNext();

    UniformsFS uniformsFS{};
    uniformsFS.alpha = 0.7f;

    uboFS->Data(uniformsFS);

    m_shader->Bind("uboVS", *uboVS);
    m_shader->Bind("uboGS", *uboGS);
    m_shader->Bind("uboFS", *uboFS);

    const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
    const VkBuffer vertexBuffers[] = { *rayCastingComponent->GetModel()->GetVertexBuffer() };
    const VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(renderContext.commandBuffer, *rayCastingComponent->GetModel()->GetIndexBuffer(), 0, rayCastingComponent->GetModel()->GetIndexBuffer()->GetIndexType());
    vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDrawIndexed(renderContext.commandBuffer, rayCastingComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
}

void RayCastDebugRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void RayCastDebugRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void RayCastDebugRenderer::ShutDown()
{
    m_pipeline = nullptr;
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::debug

#endif