#include "TerrainNormalMappedRenderer.h"

#include "../RendererUtils.h"

#include "../../../Tags.h"
#include "../../../common/ShaderAssetManager.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/ray_casting/RayCastingCommon.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/sky/SkyCommon.h"
#include "../../../component/terrain/ITerrainComponent.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::terrain {
namespace {
    constexpr uint32_t COLOR_INDEX{ 0 };
    constexpr uint32_t NORMAL_INDEX{ 1 };
} // namespace

TerrainNormalMappedRenderer::TerrainNormalMappedRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void TerrainNormalMappedRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "terrain/terrain_normal_mapped_vert") },
            { GFX_SHADER_STAGE_FRAGMENT, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "terrain/terrain_normal_mapped_frag") }
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
        .AddBindGroupEntries({
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("uboVS", 0, GFX_SHADER_STAGE_VERTEX),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("uboFS", 1, GFX_SHADER_STAGE_FRAGMENT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Texture("colorTextures", 2, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_2D_ARRAY),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Sampler("colorSampler", 3, GFX_SHADER_STAGE_FRAGMENT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Texture("normalTextures", 4, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_2D_ARRAY),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Sampler("normalSampler", 5, GFX_SHADER_STAGE_FRAGMENT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Texture("depthTexture", 6, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_2D_ARRAY, 1, GFX_TEXTURE_SAMPLE_TYPE_UNFILTERABLE_FLOAT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Sampler("depthSampler", 7, GFX_SHADER_STAGE_FRAGMENT, true)
        })
	    .SetBindGroupCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Terrain Normal Mapped Shader created");

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

    LOGI("Terrain Normal Mapped Pipeline created");

    m_uniformsPoolVS = prev::render::buffer::BufferPoolBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                           .SetMemoryProperties(GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT)
                           .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM | GFX_BUFFER_USAGE_MAP_WRITE)
                           .SetCount(m_descriptorCount)
                           .SetStride(sizeof(UniformsVS))
                           .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                           .Build();

    m_uniformsPoolFS = prev::render::buffer::BufferPoolBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                           .SetMemoryProperties(GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT)
                           .SetUsageFlags(GFX_BUFFER_USAGE_UNIFORM | GFX_BUFFER_USAGE_MAP_WRITE)
                           .SetCount(m_descriptorCount)
                           .SetStride(sizeof(UniformsFS))
                           .SetAlignment(m_device.GetGPU().GetLimits().minUniformBufferOffsetAlignment)
                           .Build();

    LOGI("Terrain Normal Mapped Uniforms Pools created");

    m_colorSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .SetAnisotropyFilterEnabled(true)
                         .Build();

    m_normalSampler = prev::render::sampler::SamplerBuilder{ m_device }
                          .Build();

    m_depthSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .SetMinFilter(GFX_FILTER_MODE_NEAREST)
                         .SetMagFilter(GFX_FILTER_MODE_NEAREST)
                         .SetMipMapMode(GFX_FILTER_MODE_NEAREST)
                         .Build();

    LOGI("Terrain Normal Mapped Samplers created");
}

void TerrainNormalMappedRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void TerrainNormalMappedRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void TerrainNormalMappedRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_TERRAIN_NORMAL_MAPPED_RENDER_COMPONENT })) {
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
    for (size_t i = 0; i < lightComponents.size(); ++i) {
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
        uniformsFS.shadows.cascades[i] = ShadowsCascadeUniform(cascade.GetBiasedViewProjectionMatrix(m_device.GetGPU().GetInfo().backend == GFX_BACKEND_WEBGPU), glm::vec4(cascade.endSplitDepth));
    }
    uniformsFS.shadows.enabled = prev_test::component::shadow::SHADOWS_ENABLED;
    uniformsFS.shadows.useReverseDepth = REVERSE_DEPTH;

    // lightning
    for (size_t i = 0; i < lightComponents.size(); ++i) {
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
        uniformsFS.heightSteps[i] = glm::vec4(terrainComponent->GetHeightSteps().at(i));
        uniformsFS.material[i] = MaterialUniform(material->GetColor(), material->GetShineDamper(), material->GetReflectivity());
    }
    uniformsFS.heightTransitionRange = terrainComponent->GetTransitionRange();
    uniformsFS.hasNormalMap = terrainComponent->GetMaterials().size() > 0 ? terrainComponent->GetMaterials().at(0)->HasImageBuffer(NORMAL_INDEX) : false;

    uboFS.Write(uniformsFS);

    m_shader->Bind("colorTextures", terrainComponent->GetTextureArray(COLOR_INDEX)->GetTextureView());
    m_shader->Bind("colorSampler", *m_colorSampler);
    m_shader->Bind("normalTextures", terrainComponent->GetTextureArray(NORMAL_INDEX)->GetTextureView());
    m_shader->Bind("normalSampler", *m_normalSampler);
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

void TerrainNormalMappedRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void TerrainNormalMappedRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void TerrainNormalMappedRenderer::ShutDown()
{
    m_depthSampler.reset();
    m_normalSampler.reset();
    m_colorSampler.reset();

    m_uniformsPoolFS.reset();
    m_uniformsPoolVS.reset();

    m_pipeline.reset();
    m_shader.reset();
}
} // namespace prev_test::render::renderer::terrain
