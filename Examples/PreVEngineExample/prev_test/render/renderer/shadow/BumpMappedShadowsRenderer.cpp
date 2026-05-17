#include "BumpMappedShadowsRenderer.h"

#include "../../IMesh.h"
#include "../RendererUtils.h"

#include "../../../Tags.h"
#include "../../../common/ShaderAssetManager.h"
#include "../../../component/render/IRenderComponent.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::shadow {
BumpMappedShadowsRenderer::BumpMappedShadowsRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void BumpMappedShadowsRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "shadow/bump_mapped_shadows_vert") }
        })
        .AddVertexInputAttributes({
            prev::render::shader::VertexInputAttribute{ 0, 0, GFX_FORMAT_R32G32B32_FLOAT, 0 },
            prev::render::shader::VertexInputAttribute{ 0, 1, GFX_FORMAT_R32G32_FLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })},
            prev::render::shader::VertexInputAttribute{ 0, 2, GFX_FORMAT_R32G32B32_FLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })},
            prev::render::shader::VertexInputAttribute{ 0, 3, GFX_FORMAT_R32G32B32_FLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })},
            prev::render::shader::VertexInputAttribute{ 0, 4, GFX_FORMAT_R32G32B32_FLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 })},
        })
        .AddVertexInputBindings({
            prev::render::shader::VertexInputBinding{ 0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), GFX_VERTEX_STEP_MODE_VERTEX }
        })
        .AddDescriptorSets({
            prev::render::shader::ShaderBuilder::DescriptorSet::Buffer("ubo", 0, GFX_SHADER_STAGE_VERTEX)
        })
	    .SetBindGroupCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Bump Mapped Shadows Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(GFX_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(false)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(GFX_POLYGON_MODE_FILL)
        .SetCullingMode(GFX_CULL_MODE_BACK)
        .Build();
    // clang-format on

    LOGI("Bump Mapped Shadows Pipeline created");

    m_uniformsPool = prev::render::buffer::BufferPoolBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                         .SetHostMapped(true)
                         .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM)
                         .SetCount(m_descriptorCount)
                         .SetStride(sizeof(Uniforms))
                         .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                         .Build();

    LOGI("Bump Mapped Shadows Uniforms Pools created");
}

void BumpMappedShadowsRenderer::BeforeRender(const ShadowsRenderContext& renderContext)
{
}

void BumpMappedShadowsRenderer::PreRender(const ShadowsRenderContext& renderContext)
{
    const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void BumpMappedShadowsRenderer::Render(const ShadowsRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAny({ TAG_RENDER_NORMAL_MAPPED_COMPONENT, TAG_RENDER_CONE_STEP_MAPPED_COMPONENT })) {
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

            m_uniformsPool->MoveToNext();

            auto& ubo = m_uniformsPool->GetCurrent();

            Uniforms uniforms{};
            uniforms.projectionMatrix = renderContext.projectionMatrix;
            uniforms.viewMatrix = renderContext.viewMatrix;
            uniforms.modelMatrix = transformComponent->GetWorldTransformScaled() * meshNode.transform;
            ubo.Write(uniforms);

            m_shader->Bind("ubo", ubo);

            const GfxBindGroup descriptorSet = m_shader->UpdateNextBindGroup();
            const uint64_t vertexOffset = static_cast<uint64_t>(meshPart.firstVertexIndex) * mesh->GetVertexLayout().GetStride();
            const uint64_t vertexRange = model->GetVertexBuffer()->GetSize() - vertexOffset;
            gfxRenderPassEncoderSetVertexBuffer(renderContext.renderPassEncoder, 0, *model->GetVertexBuffer(), vertexOffset, vertexRange);
            gfxRenderPassEncoderSetIndexBuffer(renderContext.renderPassEncoder, *model->GetIndexBuffer(), GFX_INDEX_FORMAT_UINT32, 0, model->GetIndexBuffer()->GetSize());
            gfxRenderPassEncoderSetBindGroup(renderContext.renderPassEncoder, 0, descriptorSet, nullptr, 0);

            gfxRenderPassEncoderDrawIndexed(renderContext.renderPassEncoder, meshPart.indicesCount, 1, meshPart.firstIndicesIndex, 0, 0);
        }

        for (const auto& childMeshNode : meshNode.children) {
            RenderMeshNode(childMeshNode);
        }
    };

    RenderMeshNode(renderComponent->GetModel()->GetMesh()->GetRootNode());
}

void BumpMappedShadowsRenderer::PostRender(const ShadowsRenderContext& renderContext)
{
}

void BumpMappedShadowsRenderer::AfterRender(const ShadowsRenderContext& renderContext)
{
}

void BumpMappedShadowsRenderer::ShutDown()
{
    m_uniformsPool = {};

    m_pipeline = {};
    m_shader = {};
}
} // namespace prev_test::render::renderer::shadow
