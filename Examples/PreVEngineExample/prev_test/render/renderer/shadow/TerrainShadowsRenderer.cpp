#include "TerrainShadowsRenderer.h"

#include "../RendererUtils.h"

#include "../../../Tags.h"
#include "../../../common/ShaderAssetManager.h"
#include "../../../component/terrain/ITerrainComponent.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::shadow {
TerrainShadowsRenderer::TerrainShadowsRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void TerrainShadowsRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "shadow/terrain_shadows_vert") }
        })
        .AddVertexInputAttributes({
            prev::render::shader::VertexInputAttribute{ 0, 0, GFX_FORMAT_R32G32B32_FLOAT, 0 },
            prev::render::shader::VertexInputAttribute{ 0, 1, GFX_FORMAT_R32G32_FLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })},
            prev::render::shader::VertexInputAttribute{ 0, 2, GFX_FORMAT_R32G32B32_FLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })}
        })
        .AddVertexInputBindings({
            prev::render::shader::VertexInputBinding{ 0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), GFX_VERTEX_STEP_MODE_VERTEX }
        })
        .AddBindGroupEntries({
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("ubo", 0, GFX_SHADER_STAGE_VERTEX)
        })
	    .SetBindGroupCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Terrain Shadows Shader created");

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

    LOGI("Terrain Shadows Pipeline created");

    m_uniformsPool = prev::render::buffer::BufferPoolBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                         .SetHostMapped(true)
                         .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM)
                         .SetCount(m_descriptorCount)
                         .SetStride(sizeof(Uniforms))
                         .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                         .Build();

    LOGI("Terrain Shadows Uniforms Pools created");
}

void TerrainShadowsRenderer::BeforeRender(const ShadowsRenderContext& renderContext)
{
}

void TerrainShadowsRenderer::PreRender(const ShadowsRenderContext& renderContext)
{
    const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void TerrainShadowsRenderer::Render(const ShadowsRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_TERRAIN_RENDER_COMPONENT })) {
        return;
    }

    if (!node->GetTags().HasAll({ TAG_TRANSFORM_COMPONENT })) {
        return;
    }

    if (!prev_test::render::renderer::IsVisible(&renderContext.frustum, 1, node)) {
        return;
    }

    const auto transformComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(node);
    const auto terrainComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::terrain::ITerrainComponent>(node);

    m_uniformsPool->MoveToNext();

    auto& ubo = m_uniformsPool->GetCurrent();

    Uniforms uniforms{};
    uniforms.projectionMatrix = renderContext.projectionMatrix;
    uniforms.viewMatrix = renderContext.viewMatrix;
    uniforms.modelMatrix = transformComponent->GetWorldTransformScaled();
    ubo.Write(uniforms);

    m_shader->Bind("ubo", ubo);

    const GfxBindGroup descriptorSet = m_shader->UpdateNextBindGroup();
    const uint64_t vertexOffset = 0;
    const uint64_t vertexRange = terrainComponent->GetModel()->GetVertexBuffer()->GetSize() - vertexOffset;
    gfxRenderPassEncoderSetVertexBuffer(renderContext.renderPassEncoder, 0, *terrainComponent->GetModel()->GetVertexBuffer(), vertexOffset, vertexRange);
    gfxRenderPassEncoderSetIndexBuffer(renderContext.renderPassEncoder, *terrainComponent->GetModel()->GetIndexBuffer(), GFX_INDEX_FORMAT_UINT32, 0, terrainComponent->GetModel()->GetIndexBuffer()->GetSize());
    gfxRenderPassEncoderSetBindGroup(renderContext.renderPassEncoder, 0, descriptorSet, nullptr, 0);

    gfxRenderPassEncoderDrawIndexed(renderContext.renderPassEncoder, terrainComponent->GetModel()->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);
}

void TerrainShadowsRenderer::PostRender(const ShadowsRenderContext& renderContext)
{
}

void TerrainShadowsRenderer::AfterRender(const ShadowsRenderContext& renderContext)
{
}

void TerrainShadowsRenderer::ShutDown()
{
    m_uniformsPool = {};

    m_pipeline = {};
    m_shader = {};
}
} // namespace prev_test::render::renderer::shadow
