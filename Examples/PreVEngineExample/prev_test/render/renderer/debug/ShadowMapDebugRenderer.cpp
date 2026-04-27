#include "ShadowMapDebugRenderer.h"
#include <prev/common/Logger.h>

#include "../../../Tags.h"
#include <prev/common/Logger.h>
#include "../../../common/AssetManager.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/shadow/ShadowsCommon.h"
#include "../../mesh/MeshFactory.h"
#include "../../model/ModelFactory.h"

#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::debug {
ShadowMapDebugRenderer::ShadowMapDebugRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void ShadowMapDebugRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/debug/shadow_map_debug_vert.spv") },
            { GFX_SHADER_STAGE_FRAGMENT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/debug/shadow_map_debug_frag.spv") }
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
            prev::render::shader::ShaderBuilder::DescriptorSet::Texture("depthTexture", 0, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_2D_ARRAY, 1, GFX_TEXTURE_SAMPLE_TYPE_UNFILTERABLE_FLOAT),
            prev::render::shader::ShaderBuilder::DescriptorSet::Sampler("depthSampler", 1, GFX_SHADER_STAGE_FRAGMENT, true)
        })
	    .SetBindGroupCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("ShadowMapDebug Shader created");

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

    LOGI("ShadowMapDebug Pipeline created");

    m_depthSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .SetAddressMode(GFX_ADDRESS_MODE_REPEAT)
                         .SetMinFilter(GFX_FILTER_MODE_NEAREST)
                         .SetMagFilter(GFX_FILTER_MODE_NEAREST)
                         .SetMipMapMode(GFX_FILTER_MODE_NEAREST)
                         .Build();

    LOGI("ShadowMapDebug Sampler created");

    // create quad model
    prev_test::render::mesh::MeshFactory meshFactory{};
    auto quadMesh{ meshFactory.CreateQuad() };

    prev_test::render::model::ModelFactory modelFactory{ m_device };
    m_quadModel = modelFactory.Create(std::move(quadMesh));
}

void ShadowMapDebugRenderer::BeforeRender(const prev::render::RenderContext& renderContext)
{
}

void ShadowMapDebugRenderer::PreRender(const prev::render::RenderContext& renderContext)
{
        const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

// make a node with quad model & shadowMap texture ???
void ShadowMapDebugRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    const auto shadows = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>(m_scene.GetRootNode(), { TAG_SHADOW });

    const auto& cascade{ shadows->GetCascadeFrameData(static_cast<uint32_t>(m_cascadeIndex)) };
    // TODO: Push constants not supported in gfx API - convert to UBO
    // PushConstantBlock pushConstBlock{ static_cast<uint32_t>(m_cascadeIndex), -cascade.startSplitDepth, -cascade.endSplitDepth };
    (void)cascade;

    m_shader->Bind("depthTexture", shadows->GetImageBuffer()->GetTextureView());
    m_shader->Bind("depthSampler", *m_depthSampler);

    const GfxBindGroup descriptorSet = m_shader->UpdateNextBindGroup();
        const uint64_t vertexOffset = 0;
        const uint64_t vertexRange = m_quadModel->GetVertexBuffer()->GetSize() - vertexOffset;
        gfxRenderPassEncoderSetVertexBuffer(renderContext.renderPassEncoder, 0, *m_quadModel->GetVertexBuffer(), vertexOffset, vertexRange);
    gfxRenderPassEncoderSetIndexBuffer(renderContext.renderPassEncoder, *m_quadModel->GetIndexBuffer(), GFX_INDEX_FORMAT_UINT32, 0, m_quadModel->GetIndexBuffer()->GetSize());
    gfxRenderPassEncoderSetBindGroup(renderContext.renderPassEncoder, 0, descriptorSet, nullptr, 0);

    gfxRenderPassEncoderDrawIndexed(renderContext.renderPassEncoder, m_quadModel->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);
}

void ShadowMapDebugRenderer::PostRender(const prev::render::RenderContext& renderContext)
{
}

void ShadowMapDebugRenderer::AfterRender(const prev::render::RenderContext& renderContext)
{
}

void ShadowMapDebugRenderer::ShutDown()
{
    m_depthSampler = {};

    m_pipeline = {};
    m_shader = {};
}

void ShadowMapDebugRenderer::operator()(const prev::input::keyboard::KeyEvent& keyEvent)
{
    if (keyEvent.action == prev::input::keyboard::KeyActionType::PRESS) {
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_O) {
            m_cascadeIndex = (m_cascadeIndex - 1) < 0 ? prev_test::component::shadow::CASCADES_COUNT - 1 : m_cascadeIndex - 1;
            std::cout << "New Cascade Index = " << m_cascadeIndex << std::endl;
        } else if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_P) {
            m_cascadeIndex = (m_cascadeIndex + 1) % prev_test::component::shadow::CASCADES_COUNT;
            std::cout << "New Cascade Index = " << m_cascadeIndex << std::endl;
        }
    }
}
} // namespace prev_test::render::renderer::debug
