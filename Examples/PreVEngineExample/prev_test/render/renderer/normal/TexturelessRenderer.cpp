#include "TexturelessRenderer.h"

#include "../../IMesh.h"
#include "../RendererUtils.h"

#include "../../../Tags.h"
#include "../../../common/ShaderAssetManager.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/ray_casting/ISelectableComponent.h"
#include "../../../component/ray_casting/RayCastingCommon.h"
#include "../../../component/render/IRenderComponent.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/sky/SkyCommon.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/render/buffer/BufferPoolBuilder.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::normal {
TexturelessRenderer::TexturelessRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void TexturelessRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { GFX_SHADER_STAGE_VERTEX, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "normal/textureless_vert") },
            { GFX_SHADER_STAGE_FRAGMENT, prev_test::common::ShaderAssetManager::Instance().GetAssetPath(m_device.GetGPU().GetInfo().backend, "normal/textureless_frag") }
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
            prev::render::shader::ShaderBuilder::BindGroupEntry::Buffer("uboFS", 1, GFX_SHADER_STAGE_FRAGMENT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Texture("depthTexture", 2, GFX_SHADER_STAGE_FRAGMENT, GFX_TEXTURE_VIEW_TYPE_2D_ARRAY, 1, GFX_TEXTURE_SAMPLE_TYPE_UNFILTERABLE_FLOAT),
            prev::render::shader::ShaderBuilder::BindGroupEntry::Sampler("depthSampler", 3, GFX_SHADER_STAGE_FRAGMENT, true)
        })
	    .SetBindGroupCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Textureless Shader created");

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

    LOGI("Textureless Pipeline created");

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

    LOGI("Textureless Uniforms Pools created");

    m_depthSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .SetMinFilter(GFX_FILTER_MODE_NEAREST)
                         .SetMagFilter(GFX_FILTER_MODE_NEAREST)
                         .SetMipMapMode(GFX_FILTER_MODE_NEAREST)
                         .Build();

    LOGI("Textureless Depth Sampler created");
}

void TexturelessRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void TexturelessRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const GfxViewport viewport{ static_cast<float>(renderContext.rect.origin.x), static_cast<float>(renderContext.rect.origin.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0.0f, 1.0f };

    gfxRenderPassEncoderSetPipeline(renderContext.renderPassEncoder, *m_pipeline);
    gfxRenderPassEncoderSetViewport(renderContext.renderPassEncoder, &viewport);
    gfxRenderPassEncoderSetScissorRect(renderContext.renderPassEncoder, &renderContext.rect);
}

void TexturelessRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_RENDER_TEXTURELESS_COMPONENT })) {
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
    const auto nodeRenderComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::render::IRenderComponent>(node);

    std::function<void(const prev_test::render::MeshNode&)> RenderMeshNode = [&](const prev_test::render::MeshNode& meshNode) {
        const auto model = nodeRenderComponent->GetModel();
        const auto mesh = model->GetMesh();

        const auto& meshParts{ mesh->GetMeshParts() };
        for (const auto meshPartIndex : meshNode.meshPartIndices) {
            const auto& meshPart = meshParts[meshPartIndex];
            const auto material = nodeRenderComponent->GetMaterial(meshPart.materialIndex);
            const auto modelMatrix = transformComponent->GetWorldTransformScaled() * meshNode.transform;

            m_uniformsPoolVS->MoveToNext();

            auto& uboVS = m_uniformsPoolVS->GetCurrent();

            UniformsVS uniformsVS{};
            uniformsVS.modelMatrix = modelMatrix;
            uniformsVS.normalMatrix = glm::transpose(glm::inverse(modelMatrix));
            for (uint32_t i = 0; i < renderContext.cameraCount; ++i) {
                uniformsVS.viewMatrices[i] = renderContext.viewMatrices[i];
                uniformsVS.projectionMatrices[i] = renderContext.projectionMatrices[i];
                uniformsVS.cameraPositions[i] = glm::vec4(renderContext.cameraPositions[i], 1.0f);
            }
            uniformsVS.textureNumberOfRows = material->GetAtlasNumberOfRows();
            uniformsVS.textureOffset = glm::vec4(material->GetTextureOffset(), 0.0f, 0.0f);
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
                uniformsFS.shadows.cascades[i] = ShadowsCascadeUniform(cascade.GetBiasedViewProjectionMatrix(), glm::vec4(cascade.endSplitDepth));
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

            // material
            uniformsFS.material = MaterialUniform(material->GetColor(), material->GetShineDamper(), material->GetReflectivity());

            // common
            uniformsFS.fogColor = prev_test::component::sky::FOG_COLOR;
            uniformsFS.selectedColor = prev_test::component::ray_casting::SELECTED_COLOR;
            uniformsFS.selected = uniformsFS.selected = prev_test::render::renderer::IsSelected(node);
            uniformsFS.castedByShadows = nodeRenderComponent->IsCastedByShadows();
            uboFS.Write(uniformsFS);

            m_shader->Bind("depthTexture", shadowsComponent->GetImageBuffer()->GetTextureView());
            m_shader->Bind("depthSampler", *m_depthSampler);
            m_shader->Bind("uboVS", uboVS);
            m_shader->Bind("uboFS", uboFS);

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

    RenderMeshNode(nodeRenderComponent->GetModel()->GetMesh()->GetRootNode());
}

void TexturelessRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void TexturelessRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void TexturelessRenderer::ShutDown()
{
    m_depthSampler = {};

    m_uniformsPoolFS = {};
    m_uniformsPoolVS = {};

    m_pipeline = {};
    m_shader = {};
}
} // namespace prev_test::render::renderer::normal
