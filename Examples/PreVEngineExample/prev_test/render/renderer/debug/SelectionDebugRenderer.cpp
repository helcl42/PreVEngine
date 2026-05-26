#include "SelectionDebugRenderer.h"

#ifdef RENDER_SELECTION
#include "../../../Tags.h"
#include "../../../common/ShaderAssetManager.h"
#include "../../../component/ray_casting/ISelectableComponent.h"
#include "../../mesh/MeshFactory.h"
#include "../../model/ModelFactory.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/MathUtils.h>

namespace prev_test::render::renderer::debug {
SelectionDebugRenderer::SelectionDebugRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void SelectionDebugRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "debug/selection_debug_vert") },
            { GFX_SHADER_STAGE_FRAGMENT, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "debug/selection_debug_frag") }
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
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("uboVS", 0, GFX_SHADER_STAGE_VERTEX),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("uboFS", 1, GFX_SHADER_STAGE_FRAGMENT)
        })
	    .SetBindGroupCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Selection Debug Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(GFX_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(false)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(GFX_POLYGON_MODE_LINE)
        .SetCullingMode(GFX_CULL_MODE_BACK)
        .Build();
    // clang-format on

    LOGI("Selection Debug Pipeline created");

    m_uniformsPoolVS = prev::render::buffer::BufferPoolBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                           .SetHostMapped(true)
                           .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM)
                           .SetCount(m_descriptorCount)
                           .SetStride(sizeof(UniformsVS))
                           .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                           .Build();

    m_uniformsPoolFS = prev::render::buffer::BufferPoolBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                           .SetHostMapped(true)
                           .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM)
                           .SetCount(m_descriptorCount)
                           .SetStride(sizeof(UniformsFS))
                           .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                           .Build();

    LOGI("Selection Debug Uniforms Pools created");

    prev_test::render::mesh::MeshFactory meshFactory{};
    auto mesh = meshFactory.CreateSphere(1.0f, 32, 32);

    prev_test::render::model::ModelFactory modelFactoru{ m_device };
    m_selectionPointModel = modelFactoru.Create(std::move(mesh));
}

void SelectionDebugRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void SelectionDebugRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void SelectionDebugRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_SELECTABLE_COMPONENT })) {
        return;
    }

    const auto selectableComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::ray_casting::ISelectableComponent>(node);
    if (selectableComponent->IsSelected()) {
        m_uniformsPoolVS->MoveToNext();

        auto& uboVS = m_uniformsPoolVS->GetCurrent();

        UniformsVS uniformsVS{};
        uniformsVS.modelMatrix = prev::util::math::CreateTransformationMatrix(selectableComponent->GetPostiion(), glm::quat(), 0.6f);
        for (uint32_t i = 0; i < renderContext.cameraCount; ++i) {
            uniformsVS.viewMatrices[i] = renderContext.viewMatrices[i];
            uniformsVS.projectionMatrices[i] = renderContext.projectionMatrices[i];
        }
        uboVS.Write(uniformsVS);

        m_uniformsPoolFS->MoveToNext();

        auto& uboFS = m_uniformsPoolFS->GetCurrent();

        UniformsFS uniformsFS{};
        uniformsFS.color = glm::vec4(0.0f, 1.0f, 0.0f, 0.7f);
        uboFS.Write(uniformsFS);

        m_shader->Bind("uboVS", uboVS);
        m_shader->Bind("uboFS", uboFS);

        const GfxBindGroup descriptorSet = m_shader->UpdateNextBindGroup();
        const uint64_t vertexOffset = 0;
        const uint64_t vertexRange = m_selectionPointModel->GetVertexBuffer()->GetSize() - vertexOffset;
        gfxRenderPassEncoderSetVertexBuffer(renderContext.renderPassEncoder, 0, *m_selectionPointModel->GetVertexBuffer(), vertexOffset, vertexRange);
        gfxRenderPassEncoderSetIndexBuffer(renderContext.renderPassEncoder, *m_selectionPointModel->GetIndexBuffer(), GFX_INDEX_FORMAT_UINT32, 0, m_selectionPointModel->GetIndexBuffer()->GetSize());
        gfxRenderPassEncoderSetBindGroup(renderContext.renderPassEncoder, 0, descriptorSet, nullptr, 0);

        gfxRenderPassEncoderDrawIndexed(renderContext.renderPassEncoder, m_selectionPointModel->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);
    }
}

void SelectionDebugRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void SelectionDebugRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void SelectionDebugRenderer::ShutDown()
{
    m_uniformsPoolFS.reset();
    m_uniformsPoolVS.reset();

    m_pipeline.reset();
    m_shader.reset();
}
} // namespace prev_test::render::renderer::debug

#endif