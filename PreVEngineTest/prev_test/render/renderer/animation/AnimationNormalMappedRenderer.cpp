#include "AnimationNormalMappedRenderer.h"

#include "../../IMesh.h"
#include "../RendererUtils.h"

#include "../../../Tags.h"
#include "../../../common/AssetManager.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/ray_casting/RayCastingCommon.h"
#include "../../../component/render/IAnimationRenderComponent.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/sky/SkyCommon.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/sampler/SamplerBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::animation {
namespace {
    constexpr uint32_t COLOR_INDEX{ 0 };
    constexpr uint32_t NORMAL_INDEX{ 1 };
} // namespace

AnimationNormalMappedRenderer::AnimationNormalMappedRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
    , m_scene{ scene }
{
}

void AnimationNormalMappedRenderer::Init()
{
    // clang-format off
     m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/animation/animation_normal_mapped_vert.spv") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/animation/animation_normal_mapped_frag.spv") }
        })
        .AddVertexInputAttributeDescriptions({
            prev::util::vk::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32A32_SINT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 5, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4 })),
            prev::util::vk::CreateVertexInputAttributeDescription(0, 6, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3 }))
        })
        .AddVertexInputBindingDescriptions({
            prev::util::vk::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        })
        .AddDescriptorSets( {
            { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },
            { "uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "colorSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "normalSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "depthSampler", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
        })
        .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Animation Normal Mapped Shader created");

    // clang-format off
    m_pipeline = prev::render::pipeline::GraphicsPipelineBuilder{ m_device, *m_shader, m_renderPass }
        .SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .SetDepthTestEnabled(true)
        .SetDepthWriteEnabled(true)
        .SetBlendingModeEnabled(true)
        .SetAdditiveBlendingEnabled(false)
        .SetPolygonMode(VK_POLYGON_MODE_FILL)
        .SetCullingMode(VK_CULL_MODE_BACK_BIT)
        .Build();
    // clang-format on

    LOGI("Animation Normal Mapped Pipeline created");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformRingBuffer<UniformsVS>>(m_allocator);
    m_uniformsPoolVS->UpdateCapacity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::render::buffer::UniformRingBuffer<UniformsFS>>(m_allocator);
    m_uniformsPoolFS->UpdateCapacity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    LOGI("Animation Normal Mapped Uniforms Pools created");

    m_colorSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .SetAnisotropyFilterEnabled(true)
                         .Build();

    m_normalSampler = prev::render::sampler::SamplerBuilder{ m_device }
                          .Build();

    m_depthSampler = prev::render::sampler::SamplerBuilder{ m_device }
                         .SetMinFilter(VK_FILTER_NEAREST)
                         .SetMagFilter(VK_FILTER_NEAREST)
                         .SetMipMapMode(VK_SAMPLER_MIPMAP_MODE_NEAREST)
                         .Build();

    LOGI("Animation Normal Mapped Samplers created");
}

void AnimationNormalMappedRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void AnimationNormalMappedRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void AnimationNormalMappedRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (!node->GetTags().HasAll({ TAG_ANIMATION_NORMAL_MAPPED_RENDER_COMPONENT, TAG_TRANSFORM_COMPONENT })) {
        return;
    }

    if (!prev_test::render::renderer::IsVisible(renderContext.frustums, renderContext.cameraCount, node)) {
        return;
    }

    const auto mainLightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>(m_scene.GetRootNode(), { TAG_MAIN_LIGHT });
    const auto shadowsComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>(m_scene.GetRootNode(), { TAG_SHADOW });
    const auto lightComponents = prev::scene::component::NodeComponentHelper::FindAll<prev_test::component::light::ILightComponent>(m_scene.GetRootNode(), { TAG_LIGHT });

    const auto transformComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(node);
    const auto nodeRenderComponent = prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::render::IAnimationRenderComponent>(node);

    std::function<void(const prev_test::render::MeshNode&)> RenderMeshNode = [&](const prev_test::render::MeshNode& meshNode) {
        const auto model = nodeRenderComponent->GetModel();
        const auto mesh = model->GetMesh();
        const auto animation = nodeRenderComponent->GetCurrentAnimation();

        const auto& meshParts{ mesh->GetMeshParts() };
        for (const auto meshPartIndex : meshNode.meshPartIndices) {
            const auto& meshPart = meshParts[meshPartIndex];
            const auto& animationClip = animation->GetClip(meshPartIndex);
            const auto material = nodeRenderComponent->GetMaterial(meshPart.materialIndex);
            const auto modelMatrix = transformComponent->GetWorldTransformScaled() * meshNode.transform;

            auto& uboVS = m_uniformsPoolVS->GetNext();

            UniformsVS uniformsVS{};
            const auto& bones = animationClip.GetBoneTransforms();
            for (size_t i = 0; i < bones.size(); ++i) {
                uniformsVS.bones[i] = bones[i];
            }
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

            uboVS.Data(uniformsVS);

            auto& uboFS = m_uniformsPoolFS->GetNext();

            UniformsFS uniformsFS{};
            // shadows
            for (uint32_t i = 0; i < prev_test::component::shadow::CASCADES_COUNT; ++i) {
                const auto& cascade{ shadowsComponent->GetCascade(i) };
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
            uniformsFS.selected = false;
            uniformsFS.castedByShadows = nodeRenderComponent->IsCastedByShadows();

            uboFS.Data(uniformsFS);

            m_shader->Bind("colorSampler", *material->GetImageBuffer(COLOR_INDEX), *m_colorSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            if (material->HasImageBuffer(NORMAL_INDEX)) {
                m_shader->Bind("normalSampler", *material->GetImageBuffer(NORMAL_INDEX), *m_normalSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
            m_shader->Bind("depthSampler", *shadowsComponent->GetImageBuffer(), *m_depthSampler, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
            m_shader->Bind("uboVS", uboVS);
            m_shader->Bind("uboFS", uboFS);

            const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            const VkBuffer vertexBuffers[] = { *nodeRenderComponent->GetModel()->GetVertexBuffer() };
            const VkDeviceSize offsets[] = { meshPart.firstVertexIndex * mesh->GetVertexLayout().GetStride() };

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *model->GetIndexBuffer(), 0, model->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.commandBuffer, meshPart.indicesCount, 1, meshPart.firstIndicesIndex, 0, 0);
        }

        for (const auto& childMeshNode : meshNode.children) {
            RenderMeshNode(childMeshNode);
        }
    };

    RenderMeshNode(nodeRenderComponent->GetModel()->GetMesh()->GetRootNode());
}

void AnimationNormalMappedRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void AnimationNormalMappedRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void AnimationNormalMappedRenderer::ShutDown()
{
    m_depthSampler = {};
    m_normalSampler = {};
    m_colorSampler = {};

    m_uniformsPoolFS = {};
    m_uniformsPoolVS = {};

    m_pipeline = {};
    m_shader = {};
}
} // namespace prev_test::render::renderer::animation