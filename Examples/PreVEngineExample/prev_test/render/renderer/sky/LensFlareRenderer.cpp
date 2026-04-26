#include "LensFlareRenderer.h"

#include "../../../Tags.h"
#include "../../../common/AssetManager.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/sky/ILensFlareComponent.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::sky {
LensFlareRenderer::LensFlareRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void LensFlareRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/lens_flare_vert.spv") },
            { GFX_SHADER_STAGE_FRAGMENT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/sky/lens_flare_frag.spv") }
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
            { "uboVS", 0, GFX_BINDING_TYPE_BUFFER, GFX_SHADER_STAGE_VERTEX },
            { "uboFS", 1, GFX_BINDING_TYPE_BUFFER, GFX_SHADER_STAGE_FRAGMENT },
            { "colorTexture", 2, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "colorSampler", 3, GFX_BINDING_TYPE_SAMPLER, GFX_SHADER_STAGE_FRAGMENT }
        })
        .SetBindGroupCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("LensFlare Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(GFX_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(true)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(GFX_POLYGON_MODE_FILL)
        .SetCullingMode(GFX_CULL_MODE_BACK)
        .Build();
    // clang-format on

    LOGI("LensFlare Pipeline created");

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

    LOGI("LensFlare Uniforms Pools created");

    m_colorSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .Build();

    LOGI("LensFlare Sampler created");
}

void LensFlareRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void LensFlareRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void LensFlareRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_LENS_FLARE_RENDER_COMPONENT })) {
        return;
    }

    const auto lensFlareComponent{ prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::sky::ILensFlareComponent>(node) };
    const auto lightComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>(m_scene.GetRootNode(), { TAG_MAIN_LIGHT }) };

    std::vector<glm::vec2> flarePositions[MAX_VIEW_COUNT];
    for (uint32_t viewIndex = 0; viewIndex < renderContext.cameraCount; ++viewIndex) {
        flarePositions[viewIndex] = lensFlareComponent->ComputeFlarePositions(renderContext.projectionMatrices[viewIndex], renderContext.viewMatrices[viewIndex], renderContext.cameraPositions[viewIndex], lightComponent->GetPosition());
    }

    const auto& flares{ lensFlareComponent->GetFlares() };
    const auto& flareMaterials{ lensFlareComponent->GetMaterials() };
    for (size_t i = 0; i < flares.size(); ++i) {
        const auto& flare{ flares[i] };
        const auto& flareMaterial{ flareMaterials[i] };
        const float aspectRatio{
            static_cast<float>(renderContext.rect.extent.width - renderContext.rect.origin.x) / static_cast<float>(renderContext.rect.extent.height - renderContext.rect.origin.y)
        };
        const float xScale{ flare.GetScale() };
        const float yScale{ xScale * aspectRatio };

        m_uniformsPoolVS->MoveToNext();

        auto& uboVS = m_uniformsPoolVS->GetCurrent();
        UniformsVS uniformsVS{};
        for (uint32_t viewIndex = 0; viewIndex < renderContext.cameraCount; ++viewIndex) {
            uniformsVS.translations[viewIndex] = glm::vec4(flarePositions[viewIndex][i], MIN_DEPTH, 1.0f);
        }
        uniformsVS.scale = glm::vec4(xScale, yScale, 0.0f, 0.0f);
        uboVS.Write(uniformsVS);

        m_uniformsPoolFS->MoveToNext();

        auto& uboFS = m_uniformsPoolFS->GetCurrent();
        UniformsFS uniformsFS{};
        uniformsFS.brightness = glm::vec4(m_sunVisibilityFactor);
        uboFS.Write(uniformsFS);

        m_shader->Bind("colorTexture", *flareMaterial->GetImageBuffer());
        m_shader->Bind("colorSampler", *m_colorSampler);
        m_shader->Bind("uboVS", uboVS);
        m_shader->Bind("uboFS", uboFS);

        const GfxBindGroup descriptorSet = m_shader->UpdateNextBindGroup();
        gfxRenderPassEncoderSetVertexBuffer(renderContext.renderPassEncoder, 0, *lensFlareComponent->GetModel()->GetVertexBuffer(), 0, lensFlareComponent->GetModel()->GetVertexBuffer()->GetSize());
        gfxRenderPassEncoderSetIndexBuffer(renderContext.renderPassEncoder, *lensFlareComponent->GetModel()->GetIndexBuffer(), GFX_INDEX_FORMAT_UINT32, 0, lensFlareComponent->GetModel()->GetIndexBuffer()->GetSize());
        gfxRenderPassEncoderSetBindGroup(renderContext.renderPassEncoder, 0, descriptorSet, nullptr, 0);

        gfxRenderPassEncoderDrawIndexed(renderContext.renderPassEncoder, lensFlareComponent->GetModel()->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);
    }
}

void LensFlareRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void LensFlareRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void LensFlareRenderer::ShutDown()
{
    m_colorSampler = {};

    m_uniformsPoolFS = {};
    m_uniformsPoolVS = {};

    m_pipeline = {};
    m_shader = {};
}

void LensFlareRenderer::operator()(const prev_test::render::renderer::sky::SunVisibilityEvent& evt)
{
    m_sunVisibilityFactor = evt.visibilityFactor;
}
} // namespace prev_test::render::renderer::sky
