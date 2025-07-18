#include "DefaultShadowsRenderer.h"

#include "../../IMesh.h"
#include "../RendererUtils.h"

#include "../../../Tags.h"
#include "../../../common/AssetManager.h"
#include "../../../component/render/IRenderComponent.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::shadow {
DefaultShadowsRenderer::DefaultShadowsRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void DefaultShadowsRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/shadow/default_shadows_vert.spv") }
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
            { "ubo", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT }
        })
	    .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Default Shadows Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(false)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(VK_POLYGON_MODE_FILL)
        .SetCullingMode(VK_CULL_MODE_BACK_BIT)
        .Build();
    // clang-format on

    LOGI("Default Shadows Pipeline created");

    m_uniformsPool = std::make_unique<prev::render::buffer::UniformRingBuffer<Uniforms>>(m_allocator);
    m_uniformsPool->UpdateCapacity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    LOGI("Default Shadows Uniforms Pools created");
}

void DefaultShadowsRenderer::BeforeRender(const ShadowsRenderContext& renderContext)
{
}

void DefaultShadowsRenderer::PreRender(const ShadowsRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void DefaultShadowsRenderer::Render(const ShadowsRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAny({ TAG_RENDER_COMPONENT, TAG_RENDER_TEXTURELESS_COMPONENT })) {
        return;
    }

    if (!node->GetTags().HasAll({ TAG_TRANSFORM_COMPONENT })) {
        return;
    }

    if (!prev_test::render::renderer::IsVisible(&renderContext.frustum, 1, node)) {
        return;
    }

    const auto renderComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::render::IRenderComponent>(node);
    if (!renderComponent->CastsShadows()) {
        return;
    }

    const auto transformComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(node);

    std::function<void(const prev_test::render::MeshNode&)> RenderMeshNode = [&](const prev_test::render::MeshNode& meshNode) {
        const auto model = renderComponent->GetModel();
        const auto mesh = model->GetMesh();

        const auto& meshParts{ mesh->GetMeshParts() };
        for (const auto meshPartIndex : meshNode.meshPartIndices) {
            const auto& meshPart = meshParts[meshPartIndex];
            const auto modelMatrix = transformComponent->GetWorldTransformScaled() * meshNode.transform;

            auto& ubo = m_uniformsPool->GetNext();

            Uniforms uniforms{};
            uniforms.projectionMatrix = renderContext.projectionMatrix;
            uniforms.viewMatrix = renderContext.viewMatrix;
            uniforms.modelMatrix = modelMatrix;
            ubo.Data(uniforms);

            m_shader->Bind("ubo", ubo);

            const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            const VkBuffer vertexBuffers[] = { *model->GetVertexBuffer() };
            const VkDeviceSize offsets[] = { meshPart.firstVertexIndex * mesh->GetVertexLayout().GetStride() };

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *model->GetIndexBuffer(), 0, model->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.commandBuffer, meshPart.indicesCount, 1, meshPart.firstIndicesIndex, 0, 0);
        }

        for (const auto& childMeshNode : meshNode.children) {
            RenderMeshNode(childMeshNode);
        }
    };

    RenderMeshNode(renderComponent->GetModel()->GetMesh()->GetRootNode());
}

void DefaultShadowsRenderer::PostRender(const ShadowsRenderContext& renderContext)
{
}

void DefaultShadowsRenderer::AfterRender(const ShadowsRenderContext& renderContext)
{
}

void DefaultShadowsRenderer::ShutDown()
{
    m_uniformsPool = {};

    m_pipeline = {};
    m_shader = {};
}
} // namespace prev_test::render::renderer::shadow
