#include "WaterRenderer.h"

#include "../RendererUtils.h"

#include "../../../Tags.h"
#include "../../../common/ShaderAssetManager.h"
#include "../../../component/common/IOffScreenRenderPassComponent.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/sky/SkyCommon.h"
#include "../../../component/transform/ITransformComponent.h"
#include "../../../component/water/IWaterComponent.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::water {
namespace {
    constexpr uint32_t COLOR_INDEX{ 0 };
    constexpr uint32_t NORMAL_INDEX{ 1 };
} // namespace

WaterRenderer::WaterRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void WaterRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "water/water_vert") },
            { GFX_SHADER_STAGE_FRAGMENT, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "water/water_frag") }
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
prev::render::shader::ShaderBuilder::DescriptorSet::Texture("depthTexture", 2, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_2D_ARRAY, 1, GFX_TEXTURE_SAMPLE_TYPE_UNFILTERABLE_FLOAT),
prev::render::shader::ShaderBuilder::DescriptorSet::Sampler("depthSampler", 3, GFX_SHADER_STAGE_FRAGMENT, true),
prev::render::shader::ShaderBuilder::DescriptorSet::Texture("reflectionTexture", 4, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_2D),
{ "reflectionSampler", 5, GFX_BINDING_TYPE_SAMPLER, GFX_SHADER_STAGE_FRAGMENT },
prev::render::shader::ShaderBuilder::DescriptorSet::Texture("refractionTexture", 6, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_2D),
{ "refractionSampler", 7, GFX_BINDING_TYPE_SAMPLER, GFX_SHADER_STAGE_FRAGMENT },
prev::render::shader::ShaderBuilder::DescriptorSet::Texture("depthMapTexture", 8, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_2D, 1, GFX_TEXTURE_SAMPLE_TYPE_UNFILTERABLE_FLOAT),
prev::render::shader::ShaderBuilder::DescriptorSet::Sampler("depthMapSampler", 9, GFX_SHADER_STAGE_FRAGMENT, true),
{ "dudvMapTexture", 10, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
{ "dudvMapSampler", 11, GFX_BINDING_TYPE_SAMPLER, GFX_SHADER_STAGE_FRAGMENT },
{ "normalMapTexture", 12, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
{ "normalMapSampler", 13, GFX_BINDING_TYPE_SAMPLER, GFX_SHADER_STAGE_FRAGMENT }
})
	    .SetBindGroupCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Water Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(GFX_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(true)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(GFX_POLYGON_MODE_FILL)
        .SetCullingMode(GFX_CULL_MODE_NONE)
        .Build();
    // clang-format on

    LOGI("Water Pipeline crGetSampler(eated");

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

    LOGI("Water Uniforms Pools created");

    m_colorSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .SetAddressMode(GFX_ADDRESS_MODE_REPEAT)
                         .SetAnisotropyFilterEnabled(true)
                         .Build();

    m_normalSampler = prev::render::sampler::SamplerBuilder{ m_device }
                          .SetAddressMode(GFX_ADDRESS_MODE_REPEAT)
                          .Build();

    m_depthSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .SetAddressMode(GFX_ADDRESS_MODE_REPEAT)
                         .SetMinFilter(GFX_FILTER_MODE_NEAREST)
                         .SetMagFilter(GFX_FILTER_MODE_NEAREST)
                         .SetMipMapMode(GFX_FILTER_MODE_NEAREST)
                         .Build();

    LOGI("Water Samplers created");
}

void WaterRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void WaterRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void WaterRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_WATER_RENDER_COMPONENT })) {
        return;
    }

    if (!node->GetTags().HasAll({ TAG_TRANSFORM_COMPONENT })) {
        return;
    }

    if (!prev_test::render::renderer::IsVisible(renderContext.frustums, renderContext.cameraCount, node)) {
        return;
    }

    const auto waterComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::water::IWaterComponent>(node);
    const auto transformComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(node);
    const auto waterReflectionComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>(m_scene.GetRootNode(), { TAG_WATER_REFLECTION_RENDER_COMPONENT });
    const auto waterRefractionComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>(m_scene.GetRootNode(), { TAG_WATER_REFRACTION_RENDER_COMPONENT });
    const auto mainLightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>(m_scene.GetRootNode(), { TAG_MAIN_LIGHT });
    const auto shadowsComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>(m_scene.GetRootNode(), { TAG_SHADOW });

    m_uniformsPoolVS->MoveToNext();

    auto& uboVS = m_uniformsPoolVS->GetCurrent();

    UniformsVS uniformsVS{};
    uniformsVS.modelMatrix = transformComponent->GetWorldTransformScaled();
    for (uint32_t i = 0; i < renderContext.cameraCount; ++i) {
        uniformsVS.viewMatrices[i] = renderContext.viewMatrices[i];
        uniformsVS.projectionMatrices[i] = renderContext.projectionMatrices[i];
        uniformsVS.cameraPositions[i] = glm::vec4(renderContext.cameraPositions[i], 1.0f);
    }
    uniformsVS.density = prev_test::component::sky::FOG_DENSITY;
    uniformsVS.gradient = prev_test::component::sky::FOG_GRADIENT;

    uboVS.Write(uniformsVS);

    m_uniformsPoolFS->MoveToNext();

    auto& uboFS = m_uniformsPoolFS->GetCurrent();

    UniformsFS uniformsFS{};
    uniformsFS.fogColor = prev_test::component::sky::FOG_COLOR;
    uniformsFS.waterColor = waterComponent->GetMaterial()->GetColor();
    uniformsFS.light.color = glm::vec4(mainLightComponent->GetColor(), 1.0f);
    uniformsFS.light.position = glm::vec4(mainLightComponent->GetPosition(), 1.0f);
    uniformsFS.nearFarClippingPlane = glm::vec4(renderContext.nearFarClippingPlanes[0], 0.0f, 0.0f);
    uniformsFS.moveFactor = waterComponent->GetMoveFactor();
    // shadows
    for (uint32_t i = 0; i < prev_test::component::shadow::CASCADES_COUNT; ++i) {
        const auto& cascade{ shadowsComponent->GetCascadeFrameData(i) };
        uniformsFS.shadows.cascades[i] = ShadowsCascadeUniform(cascade.GetBiasedViewProjectionMatrix(), glm::vec4(cascade.endSplitDepth));
    }
    uniformsFS.shadows.enabled = prev_test::component::shadow::SHADOWS_ENABLED;
    uniformsFS.shadows.useReverseDepth = REVERSE_DEPTH ? 1 : 0;

    uboFS.Write(uniformsFS);

    m_shader->Bind("uboVS", uboVS);
    m_shader->Bind("uboFS", uboFS);
    m_shader->Bind("depthTexture", shadowsComponent->GetImageBuffer()->GetTextureView());
    m_shader->Bind("depthSampler", *m_depthSampler);
    m_shader->Bind("reflectionTexture", waterReflectionComponent->GetColorImageBuffer()->GetTextureView());
    m_shader->Bind("reflectionSampler", *m_colorSampler);
    m_shader->Bind("refractionTexture", waterRefractionComponent->GetColorImageBuffer()->GetTextureView());
    m_shader->Bind("refractionSampler", *m_colorSampler);
    m_shader->Bind("dudvMapTexture", waterComponent->GetMaterial()->GetImageBuffer(COLOR_INDEX)->GetTextureView());
    m_shader->Bind("dudvMapSampler", *m_colorSampler);
    m_shader->Bind("normalMapTexture", waterComponent->GetMaterial()->GetImageBuffer(NORMAL_INDEX)->GetTextureView());
    m_shader->Bind("normalMapSampler", *m_normalSampler);
    m_shader->Bind("depthMapTexture", waterRefractionComponent->GetDepthImageBuffer()->GetTextureView());
    m_shader->Bind("depthMapSampler", *m_depthSampler);

    const GfxBindGroup descriptorSet = m_shader->UpdateNextBindGroup();
    const uint64_t vertexOffset = 0;
    const uint64_t vertexRange = waterComponent->GetModel()->GetVertexBuffer()->GetSize() - vertexOffset;
    gfxRenderPassEncoderSetVertexBuffer(renderContext.renderPassEncoder, 0, *waterComponent->GetModel()->GetVertexBuffer(), vertexOffset, vertexRange);
    gfxRenderPassEncoderSetIndexBuffer(renderContext.renderPassEncoder, *waterComponent->GetModel()->GetIndexBuffer(), GFX_INDEX_FORMAT_UINT32, 0, waterComponent->GetModel()->GetIndexBuffer()->GetSize());
    gfxRenderPassEncoderSetBindGroup(renderContext.renderPassEncoder, 0, descriptorSet, nullptr, 0);

    gfxRenderPassEncoderDrawIndexed(renderContext.renderPassEncoder, waterComponent->GetModel()->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);
}

void WaterRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void WaterRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void WaterRenderer::ShutDown()
{
    m_depthSampler = {};
    m_normalSampler = {};
    m_colorSampler = {};

    m_uniformsPoolFS = {};
    m_uniformsPoolVS = {};

    m_pipeline = {};
    m_shader = {};
}
} // namespace prev_test::render::renderer::water
