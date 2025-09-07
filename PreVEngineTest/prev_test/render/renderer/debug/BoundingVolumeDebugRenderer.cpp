#include "BoundingVolumeDebugRenderer.h"

#ifdef RENDER_BOUNDING_VOLUMES

#include "../../../Tags.h"
#include "../../../common/AssetManager.h"
#include "../../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../../component/ray_casting/RayCastingCommon.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::debug {
BoundingVolumeDebugRenderer::BoundingVolumeDebugRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void BoundingVolumeDebugRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/debug/bounding_volume_debug_vert.spv") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/debug/bounding_volume_debug_frag.spv") }
        })
        .AddVertexInputAttributeDescriptions({
            prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
        })
        .AddVertexInputBindingDescriptions({
            prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        })
        .AddDescriptorSets({
            { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },
            { "uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
        })
	    .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Bounding Volume Debug Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(false)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(VK_POLYGON_MODE_LINE)
        .SetCullingMode(VK_CULL_MODE_NONE)
        .Build();
    // clang-format on

    LOGI("Bounding Volume Debug Pipeline created");

    m_uniformsPoolVS = prev::render::buffer::BufferPoolBuilder{ m_allocator }
                           .SetMemoryType(prev::core::memory::MemoryType::HOST_MAPPED)
                           .SetUsageFlags(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
                           .SetCount(m_descriptorCount)
                           .SetStride(sizeof(UniformsVS))
                           .SetAlignment(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment)
                           .Build();

    m_uniformsPoolFS = prev::render::buffer::BufferPoolBuilder{ m_allocator }
                           .SetMemoryType(prev::core::memory::MemoryType::HOST_MAPPED)
                           .SetUsageFlags(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
                           .SetCount(m_descriptorCount)
                           .SetStride(sizeof(UniformsFS))
                           .SetAlignment(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment)
                           .Build();

    LOGI("Bounding Volume Debug Uniforms Pools created");
}

void BoundingVolumeDebugRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void BoundingVolumeDebugRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void BoundingVolumeDebugRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_BOUNDING_VOLUME_COMPONENT })) {
        return;
    }

    const auto boundingVolumeComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::ray_casting::IBoundingVolumeComponent>(node);

    m_uniformsPoolVS->MoveToNext();

    auto& uboVS = m_uniformsPoolVS->GetCurrent();

    UniformsVS uniformsVS{};
    uniformsVS.modelMatrix = glm::mat4(1.0f);
    for (uint32_t i = 0; i < renderContext.cameraCount; ++i) {
        uniformsVS.viewMatrices[i] = renderContext.viewMatrices[i];
        uniformsVS.projectionMatrices[i] = renderContext.projectionMatrices[i];
    }
    uboVS.Write(uniformsVS);

    m_uniformsPoolFS->MoveToNext();

    auto& uboFS = m_uniformsPoolFS->GetCurrent();

    UniformsFS uniformsFS{};
    uniformsFS.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
    uniformsFS.selectedColor = prev_test::component::ray_casting::SELECTED_COLOR;
    uniformsFS.selected = false;
    uboFS.Write(uniformsFS);

    m_shader->Bind("uboVS", uboVS);
    m_shader->Bind("uboFS", uboFS);

    const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
    const VkBuffer vertexBuffers[] = { *boundingVolumeComponent->GetModel()->GetVertexBuffer() };
    const VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(renderContext.commandBuffer, *boundingVolumeComponent->GetModel()->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDrawIndexed(renderContext.commandBuffer, boundingVolumeComponent->GetModel()->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);
}

void BoundingVolumeDebugRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void BoundingVolumeDebugRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void BoundingVolumeDebugRenderer::ShutDown()
{
    m_uniformsPoolFS = {};
    m_uniformsPoolVS = {};

    m_pipeline = {};
    m_shader = {};
}
} // namespace prev_test::render::renderer::debug

#endif