#include "TextureDebugRenderer.h"

#include "../../../Tags.h"
#include <prev/common/Logger.h>
#include "../../../common/ShaderAssetManager.h"
#include "../../../component/common/IOffScreenRenderPassComponent.h"
#include "../../mesh/MeshFactory.h"
#include "../../model/ModelFactory.h"

#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::debug {
TextureDebugRenderer::TextureDebugRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void TextureDebugRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "debug/texture_debug_vert") },
            { GFX_SHADER_STAGE_FRAGMENT, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "debug/texture_debug_frag") }
        })
        .AddVertexInputAttributes({
            prev::render::shader::VertexInputAttribute{ 0, 0, GFX_FORMAT_R32G32B32_FLOAT, 0 },
            prev::render::shader::VertexInputAttribute{ 0, 1, GFX_FORMAT_R32G32_FLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })},
            prev::render::shader::VertexInputAttribute{ 0, 2, GFX_FORMAT_R32G32B32_FLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })}
        })
        .AddVertexInputBindings({
            prev::render::shader::VertexInputBinding{ 0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), GFX_VERTEX_STEP_MODE_VERTEX }
        })
        .AddDescriptorSets({
            { "imageTexture", 0, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "imageSampler", 1, GFX_BINDING_TYPE_SAMPLER, GFX_SHADER_STAGE_FRAGMENT }
        })
	    .SetBindGroupCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Texture Debug Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(GFX_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(false)
        .SetDepthWriteEnabled(false)
        .SetBlendingModeEnabled(false)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(GFX_POLYGON_MODE_LINE)
        .SetCullingMode(GFX_CULL_MODE_NONE)
        .Build();
    // clang-format on

    LOGI("Texture Debug Pipeline created");

    m_colorSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .Build();

    LOGI("Creating Texture Debug Sampler");

    // create quad model
    prev_test::render::mesh::MeshFactory meshFactory{};
    auto quadMesh{ meshFactory.CreateQuad() };

    prev_test::render::model::ModelFactory modelFactory{ m_device };
    m_quadModel = modelFactory.Create(std::move(quadMesh));
}

void TextureDebugRenderer::BeforeRender(const prev::render::RenderContext& renderContext)
{
}

void TextureDebugRenderer::PreRender(const prev::render::RenderContext& renderContext)
{
        const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void TextureDebugRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    const auto component = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>(m_scene.GetRootNode(), { TAG_WATER_REFLECTION_RENDER_COMPONENT });

    m_shader->Bind("imageTexture", component->GetColorImageBuffer()->GetTextureView());
    m_shader->Bind("imageSampler", *m_colorSampler);

    const GfxBindGroup descriptorSet = m_shader->UpdateNextBindGroup();
        const uint64_t vertexOffset = 0;
        const uint64_t vertexRange = m_quadModel->GetVertexBuffer()->GetSize() - vertexOffset;
        gfxRenderPassEncoderSetVertexBuffer(renderContext.renderPassEncoder, 0, *m_quadModel->GetVertexBuffer(), vertexOffset, vertexRange);
    gfxRenderPassEncoderSetIndexBuffer(renderContext.renderPassEncoder, *m_quadModel->GetIndexBuffer(), GFX_INDEX_FORMAT_UINT32, 0, m_quadModel->GetIndexBuffer()->GetSize());
    gfxRenderPassEncoderSetBindGroup(renderContext.renderPassEncoder, 0, descriptorSet, nullptr, 0);

    gfxRenderPassEncoderDrawIndexed(renderContext.renderPassEncoder, m_quadModel->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);
}

void TextureDebugRenderer::PostRender(const prev::render::RenderContext& renderContext)
{
}

void TextureDebugRenderer::AfterRender(const prev::render::RenderContext& renderContext)
{
}

void TextureDebugRenderer::ShutDown()
{
    m_colorSampler = {};

    m_pipeline = {};
    m_shader = {};
}
} // namespace prev_test::render::renderer::debug
