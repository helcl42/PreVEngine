#include "AnimationConeStepMappedRenderer.h"

#include "../RendererUtils.h"

#include "../../../common/AssetManager.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/ray_casting/ISelectableComponent.h"
#include "../../../component/ray_casting/RayCastingCommon.h"
#include "../../../component/render/IAnimationRenderComponent.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/sky/SkyCommon.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/render/pipeline/PipelineBuilder.h>
#include <prev/render/shader/ShaderBuilder.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer::animation {
namespace {
    constexpr uint32_t COLOR_INDEX{0};
    constexpr uint32_t NORMAL_INDEX{1};
    constexpr uint32_t HEIGHT_AND_CONE_INDEX{2};
}

AnimationConeStepMappedRenderer::AnimationConeStepMappedRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
{
}

void AnimationConeStepMappedRenderer::Init()
{
    // clang-format off
    m_shader = prev::render::shader::ShaderBuilder{ m_device }
        .AddShaderStagePaths({
            { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/animation/animation_cone_step_mapped_vert.spv") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/animation/animation_cone_step_mapped_frag.spv") }
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
        .AddDescriptorSets({
            { "uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT },
            { "uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "colorSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "normalSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "heightSampler", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT },
            { "depthSampler", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT }
        })
        .SetDescriptorPoolCapacity(m_descriptorCount)
        .Build();
    // clang-format on

    LOGI("Animation Cone Step Mapped Shader created");

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

    LOGI("Animation Cone Step Mapped Pipeline created");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsVS>>(m_allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsFS>>(m_allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(m_device.GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));
}

void AnimationConeStepMappedRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void AnimationConeStepMappedRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void AnimationConeStepMappedRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_ANIMATION_CONE_STEP_MAPPED_RENDER_COMPONENT, TAG_TRANSFORM_COMPONENT })) {
        if (prev_test::render::renderer::IsVisible(renderContext.frustums, renderContext.cameraCount, node->GetId())) {
            const auto nodeRenderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IAnimationRenderComponent>::Instance().Get(node->GetId());
            RenderMeshNode(renderContext, node, nodeRenderComponent->GetModel()->GetMesh()->GetRootNode());
        }
    }
}

void AnimationConeStepMappedRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void AnimationConeStepMappedRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void AnimationConeStepMappedRenderer::ShutDown()
{
    m_pipeline = nullptr;
    m_shader = nullptr;
}

void AnimationConeStepMappedRenderer::RenderMeshNode(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const prev_test::render::MeshNode& meshNode)
{
    const auto mainLightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT });
    const auto shadowsComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>({ TAG_SHADOW });
    const auto lightComponents = prev::scene::component::NodeComponentHelper::FindAll<prev_test::component::light::ILightComponent>({ TAG_LIGHT });

    const auto transformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(node->GetId());
    const auto nodeRenderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IAnimationRenderComponent>::Instance().Get(node->GetId());

    const auto model = nodeRenderComponent->GetModel();
    const auto mesh = model->GetMesh();
    const auto animation = nodeRenderComponent->GetCurrentAnimation();

    const auto& meshParts{ mesh->GetMeshParts() };
    for (const auto meshPartIndex : meshNode.meshPartIndices) {
        const auto& meshPart = meshParts[meshPartIndex];
        const auto& animationPart = animation->GetAnimationPart(meshPartIndex);
        const auto material = nodeRenderComponent->GetMaterial(meshPart.materialIndex);
        const auto modelMatrix = transformComponent->GetWorldTransformScaled() * meshNode.transform;

        auto uboVS = m_uniformsPoolVS->GetNext();

        UniformsVS uniformsVS{};
        const auto& bones = animationPart->GetBoneTransforms();
        for (size_t i = 0; i < bones.size(); ++i) {
            uniformsVS.bones[i] = bones[i];
        }
        uniformsVS.modelMatrix = modelMatrix;
        uniformsVS.normalMatrix = glm::transpose(glm::inverse(modelMatrix));
        for(uint32_t i = 0; i < renderContext.cameraCount; ++i) {
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
        uniformsVS.textureNumberOfRows = material->GetAtlasNumberOfRows();
        uniformsVS.textureOffset = glm::vec4(material->GetTextureOffset(), 0.0f, 0.0f);
        uniformsVS.density = prev_test::component::sky::FOG_DENSITY;
        uniformsVS.gradient = prev_test::component::sky::FOG_GRADIENT;
        uniformsVS.clipPlane = renderContext.clipPlane;

        uboVS->Update(&uniformsVS);

        auto uboFS = m_uniformsPoolFS->GetNext();

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
        uniformsFS.heightScale = material->GetHeightScale();
        uniformsFS.numLayers = 15;

        uboFS->Update(&uniformsFS);

        m_shader->Bind("colorSampler", *material->GetImageBuffer(COLOR_INDEX), *material->GetSampler(COLOR_INDEX), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        if (material->HasImageBuffer(NORMAL_INDEX)) {
            m_shader->Bind("normalSampler", *material->GetImageBuffer(NORMAL_INDEX), *material->GetSampler(NORMAL_INDEX), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        if (material->HasImageBuffer(HEIGHT_AND_CONE_INDEX)) {
            m_shader->Bind("heightSampler", *material->GetImageBuffer(HEIGHT_AND_CONE_INDEX), *material->GetSampler(HEIGHT_AND_CONE_INDEX), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        m_shader->Bind("depthSampler", *shadowsComponent->GetImageBuffer(), *shadowsComponent->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
        m_shader->Bind("uboVS", *uboVS);
        m_shader->Bind("uboFS", *uboFS);

        const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *model->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { meshPart.firstVertexIndex * mesh->GetVertexLayout().GetStride() };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *model->GetIndexBuffer(), 0, model->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, meshPart.indicesCount, 1, meshPart.firstIndicesIndex, 0, 0);
    }

    for (const auto& childMeshNode : meshNode.children) {
        RenderMeshNode(renderContext, node, childMeshNode);
    }
}
} // namespace prev_test::render::renderer::animation
