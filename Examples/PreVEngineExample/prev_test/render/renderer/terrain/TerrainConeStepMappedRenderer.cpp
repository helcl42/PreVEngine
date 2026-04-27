#include "TerrainConeStepMappedRenderer.h"

#include "../RendererUtils.h"

#include "../../../Tags.h"
#include "../../../common/AssetManager.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/ray_casting/RayCastingCommon.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/sky/SkyCommon.h"
#include "../../../component/terrain/ITerrainComponent.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::terrain {
namespace {
    constexpr uint32_t COLOR_INDEX{ 0 };
    constexpr uint32_t NORMAL_INDEX{ 1 };
    constexpr uint32_t HEIGHT_AND_CONE_INDEX{ 2 };
} // namespace

TerrainConeStepMappedRenderer::TerrainConeStepMappedRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void TerrainConeStepMappedRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/terrain/terrain_cone_step_mapped_vert.spv") },
            { GFX_SHADER_STAGE_FRAGMENT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/terrain/terrain_cone_step_mapped_frag.spv") }
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
            { "uboVS", 0, GFX_BINDING_TYPE_BUFFER, GFX_SHADER_STAGE_VERTEX },
            { "uboFS", 1, GFX_BINDING_TYPE_BUFFER, GFX_SHADER_STAGE_FRAGMENT },
            { "colorTexture0", 2, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "colorTexture1", 3, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "colorTexture2", 4, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "colorTexture3", 5, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "colorSampler", 6, GFX_BINDING_TYPE_SAMPLER, GFX_SHADER_STAGE_FRAGMENT },
            { "normalTexture0", 7, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "normalTexture1", 8, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "normalTexture2", 9, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "normalTexture3", 10, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "normalSampler", 11, GFX_BINDING_TYPE_SAMPLER, GFX_SHADER_STAGE_FRAGMENT },
            { "heightTexture0", 12, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "heightTexture1", 13, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "heightTexture2", 14, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "heightTexture3", 15, GFX_BINDING_TYPE_TEXTURE, GFX_SHADER_STAGE_FRAGMENT },
            { "heightSampler", 16, GFX_BINDING_TYPE_SAMPLER, GFX_SHADER_STAGE_FRAGMENT },
            prev::render::shader::ShaderBuilder::DescriptorSet::Texture("depthTexture", 17, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_2D_ARRAY, 1, GFX_TEXTURE_SAMPLE_TYPE_UNFILTERABLE_FLOAT),
            prev::render::shader::ShaderBuilder::DescriptorSet::Sampler("depthSampler", 18, GFX_SHADER_STAGE_FRAGMENT, true)
        })
	    .SetBindGroupCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Terrain Cone Step Mapped Shader created");

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

    LOGI("Terrain Cone Step Mapped Pipeline created");

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

    LOGI("Terrain Cone Step Mapped Uniforms Pools created");

    m_colorSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .SetAddressMode(GFX_ADDRESS_MODE_REPEAT)
                         .SetAnisotropyFilterEnabled(true)
                         .Build();

    m_normalSampler = prev::render::sampler::SamplerBuilder{ m_device }
                          .SetAddressMode(GFX_ADDRESS_MODE_REPEAT)
                          .Build();

    m_coneSampler = prev::render::sampler::SamplerBuilder{ m_device }
                        .SetAddressMode(GFX_ADDRESS_MODE_REPEAT)
                        .Build();

    m_depthSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .SetMinFilter(GFX_FILTER_MODE_NEAREST)
                         .SetMagFilter(GFX_FILTER_MODE_NEAREST)
                         .SetMipMapMode(GFX_FILTER_MODE_NEAREST)
                         .Build();

    LOGI("Terrain Cone Step Mapped Samplers created");

    m_nullImage = prev::render::buffer::ImageBufferBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                      .SetExtent({ 1, 1, 1 })
                      .SetType(GFX_TEXTURE_TYPE_2D)
                      .SetFormat(GFX_FORMAT_R8G8B8A8_UNORM)
                      .SetSampleCount(GFX_SAMPLE_COUNT_1)
                      .SetUsageFlags(GFX_TEXTURE_USAGE_TEXTURE_BINDING)
                      .SetLayout(GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY)
                      .Build();
}

void TerrainConeStepMappedRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void TerrainConeStepMappedRenderer::PreRender(const NormalRenderContext& renderContext)
{
        const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void TerrainConeStepMappedRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_TERRAIN_CONE_STEP_MAPPED_RENDER_COMPONENT })) {
        return;
    }

    if (!node->GetTags().HasAll({ TAG_TRANSFORM_COMPONENT })) {
        return;
    }

    if (!prev_test::render::renderer::IsVisible(renderContext.frustums, renderContext.cameraCount, node)) {
        return;
    }

    const auto mainLightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>(m_scene.GetRootNode(), { TAG_MAIN_LIGHT });
    const auto shadowsComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>(m_scene.GetRootNode(), { TAG_SHADOW });
    const auto lightComponents = prev::scene::component::NodeComponentHelper::FindAll<prev_test::component::light::ILightComponent>(m_scene.GetRootNode(), { TAG_LIGHT });

    const auto transformComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(node);
    const auto terrainComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::terrain::ITerrainComponent>(node);

    m_uniformsPoolVS->MoveToNext();

    auto& uboVS = m_uniformsPoolVS->GetCurrent();

    UniformsVS uniformsVS{};
    uniformsVS.modelMatrix = transformComponent->GetWorldTransformScaled();
    uniformsVS.normalMatrix = glm::transpose(glm::inverse(transformComponent->GetWorldTransformScaled()));
    for (uint32_t i = 0; i < renderContext.cameraCount; ++i) {
        uniformsVS.viewMatrices[i] = renderContext.viewMatrices[i];
        uniformsVS.projectionMatrices[i] = renderContext.projectionMatrices[i];
        uniformsVS.cameraPositions[i] = glm::vec4(renderContext.cameraPositions[i], 1.0f);
    }
    for (size_t i = 0; i < lightComponents.size(); i++) {
        const auto& lightComponent{ lightComponents[i] };
        uniformsVS.lightning.lights[i] = LightUniform(glm::vec4(lightComponent->GetPosition(), 1.0f), glm::vec4(lightComponent->GetColor(), 1.0f), glm::vec4(lightComponent->GetAttenuation(), 1.0f));
    }
    uniformsVS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
    uniformsVS.lightning.ambientFactor = prev_test::component::light::AMBIENT_LIGHT_INTENSITY;
    uniformsVS.density = prev_test::component::sky::FOG_DENSITY;
    uniformsVS.gradient = prev_test::component::sky::FOG_GRADIENT;
    uniformsVS.clipPlane = renderContext.clipPlane;

    uboVS.Write(uniformsVS);

    m_uniformsPoolFS->MoveToNext();

    auto& uboFS = m_uniformsPoolFS->GetCurrent();

    UniformsFS uniformsFS{};
    // shadows
    for (uint32_t i = 0; i < prev_test::component::shadow::CASCADES_COUNT; ++i) {
        const auto& cascade{ shadowsComponent->GetCascadeFrameData(i) };
        uniformsFS.shadows.cascades[i] = ShadowsCascadeUniform(cascade.GetBiasedViewProjectionMatrix(), glm::vec4(cascade.endSplitDepth));
    }
    uniformsFS.shadows.enabled = prev_test::component::shadow::SHADOWS_ENABLED;
    uniformsFS.shadows.useReverseDepth = REVERSE_DEPTH;

    // lightning
    for (size_t i = 0; i < lightComponents.size(); i++) {
        const auto& lightComponent{ lightComponents[i] };
        uniformsFS.lightning.lights[i] = LightUniform(glm::vec4(lightComponent->GetPosition(), 1.0f), glm::vec4(lightComponent->GetColor(), 1.0f), glm::vec4(lightComponent->GetAttenuation(), 1.0f));
    }
    uniformsFS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
    uniformsFS.lightning.ambientFactor = prev_test::component::light::AMBIENT_LIGHT_INTENSITY;

    // common
    uniformsFS.fogColor = prev_test::component::sky::FOG_COLOR;
    uniformsFS.selectedColor = prev_test::component::ray_casting::SELECTED_COLOR;
    uniformsFS.selected = false;
    uniformsFS.castedByShadows = true;
    uniformsFS.minHeight = terrainComponent->GetHeightMapInfo()->GetGlobalMinHeight();
    uniformsFS.maxHeight = terrainComponent->GetHeightMapInfo()->GetGlobalMaxHeight();
    for (size_t i = 0; i < terrainComponent->GetMaterials().size(); i++) {
        const auto material{ terrainComponent->GetMaterials().at(i) };
        uniformsFS.material[i] = MaterialUniform(material->GetColor(), material->GetShineDamper(), material->GetReflectivity());
        uniformsFS.heightScale[i] = glm::vec4(material->GetHeightScale());
        uniformsFS.heightSteps[i] = glm::vec4(terrainComponent->GetHeightSteps().at(i));
    }
    uniformsFS.heightTransitionRange = terrainComponent->GetTransitionRange();
    uniformsFS.numLayers = 15;
    uniformsFS.hasNormalMap = terrainComponent->GetMaterials().size() > 0 ? terrainComponent->GetMaterials().at(0)->HasImageBuffer(NORMAL_INDEX) : false;
    uniformsFS.hasConeMap = terrainComponent->GetMaterials().size() > 0 ? terrainComponent->GetMaterials().at(0)->HasImageBuffer(HEIGHT_AND_CONE_INDEX) : false;

    uboFS.Write(uniformsFS);

    for (size_t i = 0; i < terrainComponent->GetMaterials().size(); i++) {
        const auto material{ terrainComponent->GetMaterials().at(i) };
        m_shader->Bind("colorTexture" + std::to_string(i), material->GetImageBuffer(COLOR_INDEX)->GetTextureView());
        m_shader->Bind("normalTexture" + std::to_string(i), material->HasImageBuffer(NORMAL_INDEX) ? material->GetImageBuffer(NORMAL_INDEX)->GetTextureView() : m_nullImage->GetTextureView());
        m_shader->Bind("heightTexture" + std::to_string(i), material->HasImageBuffer(HEIGHT_AND_CONE_INDEX) ? material->GetImageBuffer(HEIGHT_AND_CONE_INDEX)->GetTextureView() : m_nullImage->GetTextureView());
    }
    m_shader->Bind("colorSampler", *m_colorSampler);
    m_shader->Bind("normalSampler", *m_normalSampler);
    m_shader->Bind("heightSampler", *m_coneSampler);
    m_shader->Bind("depthTexture", shadowsComponent->GetImageBuffer()->GetTextureView());
    m_shader->Bind("depthSampler", *m_depthSampler);
    m_shader->Bind("uboVS", uboVS);
    m_shader->Bind("uboFS", uboFS);

    const GfxBindGroup descriptorSet = m_shader->UpdateNextBindGroup();
        const uint64_t vertexOffset = 0;
        const uint64_t vertexRange = terrainComponent->GetModel()->GetVertexBuffer()->GetSize() - vertexOffset;
        gfxRenderPassEncoderSetVertexBuffer(renderContext.renderPassEncoder, 0, *terrainComponent->GetModel()->GetVertexBuffer(), vertexOffset, vertexRange);
    gfxRenderPassEncoderSetIndexBuffer(renderContext.renderPassEncoder, *terrainComponent->GetModel()->GetIndexBuffer(), GFX_INDEX_FORMAT_UINT32, 0, terrainComponent->GetModel()->GetIndexBuffer()->GetSize());
    gfxRenderPassEncoderSetBindGroup(renderContext.renderPassEncoder, 0, descriptorSet, nullptr, 0);

    gfxRenderPassEncoderDrawIndexed(renderContext.renderPassEncoder, terrainComponent->GetModel()->GetMesh()->GetIndicesCount(), 1, 0, 0, 0);
}

void TerrainConeStepMappedRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void TerrainConeStepMappedRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void TerrainConeStepMappedRenderer::ShutDown()
{
    m_nullImage = {};

    m_depthSampler = {};
    m_coneSampler = {};
    m_normalSampler = {};
    m_colorSampler = {};

    m_uniformsPoolFS = {};
    m_uniformsPoolVS = {};

    m_pipeline = {};
    m_shader = {};
}
} // namespace prev_test::render::renderer::terrain
