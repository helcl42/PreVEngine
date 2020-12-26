#include "AnimationRenderer.h"
#include "pipeline/AnimationPipeline.h"
#include "shader/AnimationShader.h"

#include "../../../component/light/ILightComponent.h"
#include "../../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../../component/ray_casting/RayCastingCommon.h"
#include "../../../component/render/IAnimationRenderComponent.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/sky/SkyCommon.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/AllocatorProvider.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::animation {
AnimationRenderer::AnimationRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void AnimationRenderer::Init()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::AnimationShader>(*device, shader::AnimationShader::GetPaths());
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Animation Shader created\n");

    m_pipeline = std::make_unique<pipeline::AnimationPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("Animation Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsVS> >(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsFS> >(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
}

void AnimationRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void AnimationRenderer::PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { 0, 0 }, renderContext.fullExtent };
    const VkViewport viewport{ 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void AnimationRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const NormalRenderContextUserData& renderContextUserData)
{
    if (node->GetTags().HasAll({ TAG_ANIMATION_RENDER_COMPONENT, TAG_TRANSFORM_COMPONENT })) {
        bool visible = true;
        if (prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
            visible = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContextUserData.frustum);
        }

        if (visible) {
            const auto nodeRenderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IAnimationRenderComponent>::Instance().Get(node->GetId());
            RenderMeshNode(renderContext, node, renderContextUserData, nodeRenderComponent->GetModel()->GetMesh()->GetRootNode());
        }
    }

    for (auto child : node->GetChildren()) {
        Render(renderContext, child, renderContextUserData);
    }
}

void AnimationRenderer::PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void AnimationRenderer::AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void AnimationRenderer::ShutDown()
{
    m_shader->ShutDown();

    m_pipeline->ShutDown();
}

void AnimationRenderer::RenderMeshNode(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const NormalRenderContextUserData& renderContextUserData, const prev_test::render::MeshNode& meshNode)
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
        const auto& meshPart = meshParts.at(meshPartIndex);
        const auto& animationPart = animation->GetAnimationPart(meshPartIndex);
        const auto material = nodeRenderComponent->GetMaterial(meshPart.materialIndex);
        const auto modelMatrix = transformComponent->GetWorldTransformScaled() * meshNode.transform;

        auto uboVS = m_uniformsPoolVS->GetNext();

        UniformsVS uniformsVS{};
        const auto& bones = animationPart->GetBoneTransforms();
        for (size_t i = 0; i < bones.size(); i++) {
            uniformsVS.bones[i] = bones[i];
        }
        uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
        uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
        uniformsVS.modelMatrix = modelMatrix;
        uniformsVS.normalMatrix = glm::inverse(modelMatrix);
        uniformsVS.textureNumberOfRows = material->GetAtlasNumberOfRows();
        uniformsVS.textureOffset = glm::vec4(material->GetTextureOffset(), 0.0f, 0.0f);
        uniformsVS.cameraPosition = glm::vec4(renderContextUserData.cameraPosition, 1.0f);
        for (size_t i = 0; i < lightComponents.size(); i++) {
            uniformsVS.lightning.lights[i] = LightUniform(glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
        }
        uniformsVS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
        uniformsVS.lightning.ambientFactor = prev_test::component::light::AMBIENT_LIGHT_INTENSITY;
        uniformsVS.useFakeLightning = material->UsesFakeLightning();
        uniformsVS.density = prev_test::component::sky::FOG_DENSITY;
        uniformsVS.gradient = prev_test::component::sky::FOG_GRADIENT;
        uniformsVS.clipPlane = renderContextUserData.clipPlane;

        uboVS->Update(&uniformsVS);

        auto uboFS = m_uniformsPoolFS->GetNext();

        UniformsFS uniformsFS{};
        // shadows
        for (uint32_t i = 0; i < prev_test::component::shadow::CASCADES_COUNT; i++) {
            auto& cascade = shadowsComponent->GetCascade(i);
            uniformsFS.shadows.cascades[i] = ShadowsCascadeUniform(cascade.GetBiasedViewProjectionMatrix(), glm::vec4(cascade.endSplitDepth));
        }
        uniformsFS.shadows.enabled = prev_test::component::shadow::SHADOWS_ENABLED;

        // lightning
        for (size_t i = 0; i < lightComponents.size(); i++) {
            uniformsFS.lightning.lights[i] = LightUniform(glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
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

        uboFS->Update(&uniformsFS);

        m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), shadowsComponent->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
        m_shader->Bind("textureSampler", *material->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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
        RenderMeshNode(renderContext, node, renderContextUserData, childMeshNode);
    }
}
} // namespace prev_test::render::renderer::animation
