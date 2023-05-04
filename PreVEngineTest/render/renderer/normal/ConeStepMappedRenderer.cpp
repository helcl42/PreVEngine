#include "ConeStepMappedRenderer.h"
#include "pipeline/ConeStepMappedPipeline.h"
#include "shader/ConeStepMappedShader.h"

#include "../../../component/render/IRenderComponent.h"
#include "../../../component/transform/ITransformComponent.h"

#include "../../../component/light/ILightComponent.h"
#include "../../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../../component/ray_casting/ISelectableComponent.h"
#include "../../../component/ray_casting/RayCastingCommon.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/sky/SkyCommon.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::normal {
ConeStepMappedRenderer::ConeStepMappedRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void ConeStepMappedRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::ConeStepMappedShader>(*device, shader::ConeStepMappedShader::GetPaths());
    m_shader->Init();
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Cone Step Mapped Shader created\n");

    m_pipeline = std::make_unique<pipeline::ConeStepMappedPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("Cone Step Mapped Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsVS>>(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsFS>>(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));
}

void ConeStepMappedRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void ConeStepMappedRenderer::PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void ConeStepMappedRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const NormalRenderContextUserData& renderContextUserData)
{
    if (node->GetTags().HasAll({ TAG_RENDER_CONE_STEP_MAPPED_COMPONENT, TAG_TRANSFORM_COMPONENT })) {
        bool visible{ true };
        if (prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
            visible = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContextUserData.frustum);
        }

        if (visible) {
            const auto nodeRenderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IRenderComponent>::Instance().Get(node->GetId());
            RenderMeshNode(renderContext, node, renderContextUserData, nodeRenderComponent->GetModel()->GetMesh()->GetRootNode());
        }
    }

    for (auto& child : node->GetChildren()) {
        Render(renderContext, child, renderContextUserData);
    }
}

void ConeStepMappedRenderer::PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void ConeStepMappedRenderer::AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void ConeStepMappedRenderer::ShutDown()
{
    m_pipeline->ShutDown();
    m_pipeline = nullptr;

    m_shader->ShutDown();
    m_shader = nullptr;
}

void ConeStepMappedRenderer::RenderMeshNode(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const NormalRenderContextUserData& renderContextUserData, const prev_test::render::MeshNode& meshNode)
{
    const auto mainLightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT });
    const auto shadowsComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>({ TAG_SHADOW });
    const auto lightComponents = prev::scene::component::NodeComponentHelper::FindAll<prev_test::component::light::ILightComponent>({ TAG_LIGHT });

    const auto transformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(node->GetId());
    const auto nodeRenderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IRenderComponent>::Instance().Get(node->GetId());

    const auto model = nodeRenderComponent->GetModel();
    const auto mesh = model->GetMesh();

    const auto& meshParts{ mesh->GetMeshParts() };
    for (const auto meshPartIndex : meshNode.meshPartIndices) {
        const auto& meshPart{ meshParts.at(meshPartIndex) };
        const auto material = nodeRenderComponent->GetMaterial(meshPart.materialIndex);
        const auto modelMatrix = transformComponent->GetWorldTransformScaled() * meshNode.transform;

        auto uboVS = m_uniformsPoolVS->GetNext();

        UniformsVS uniformsVS{};
        uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
        uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
        uniformsVS.modelMatrix = modelMatrix;
        uniformsVS.normalMatrix = glm::inverse(modelMatrix);
        uniformsVS.cameraPosition = glm::vec4(renderContextUserData.cameraPosition, 1.0f);
        for (size_t i = 0; i < lightComponents.size(); i++) {
            const auto& lightComponent{ lightComponents[i] };
            uniformsVS.lightning.lights[i] = LightUniform(glm::vec4(lightComponent->GetPosition(), 1.0f), glm::vec4(lightComponent->GetColor(), 1.0f), glm::vec4(lightComponent->GetAttenuation(), 1.0f));
        }
        uniformsVS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
        uniformsVS.lightning.ambientFactor = prev_test::component::light::AMBIENT_LIGHT_INTENSITY;
        uniformsVS.textureNumberOfRows = material->GetAtlasNumberOfRows();
        uniformsVS.textureOffset = glm::vec4(material->GetTextureOffset(), 0.0f, 0.0f);
        uniformsVS.density = prev_test::component::sky::FOG_DENSITY;
        uniformsVS.gradient = prev_test::component::sky::FOG_GRADIENT;
        uniformsVS.clipPlane = renderContextUserData.clipPlane;

        uboVS->Update(&uniformsVS);

        auto uboFS = m_uniformsPoolFS->GetNext();

        UniformsFS uniformsFS{};
        // shadows
        for (uint32_t i = 0; i < prev_test::component::shadow::CASCADES_COUNT; i++) {
            const auto& cascade{ shadowsComponent->GetCascade(i) };
            uniformsFS.shadows.cascades[i] = ShadowsCascadeUniform(cascade.GetBiasedViewProjectionMatrix(), glm::vec4(cascade.endSplitDepth));
        }
        uniformsFS.shadows.enabled = prev_test::component::shadow::SHADOWS_ENABLED;

        // lightning
        for (size_t i = 0; i < lightComponents.size(); i++) {
            const auto& lightComponent{ lightComponents[i] };
            uniformsFS.lightning.lights[i] = LightUniform(glm::vec4(lightComponent->GetPosition(), 1.0f), glm::vec4(lightComponent->GetColor(), 1.0f), glm::vec4(lightComponent->GetAttenuation(), 1.0f));
        }
        uniformsFS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
        uniformsFS.lightning.ambientFactor = prev_test::component::light::AMBIENT_LIGHT_INTENSITY;

        // material
        uniformsFS.material = MaterialUniform(material->GetColor(), material->GetShineDamper(), material->GetReflectivity());

        bool selected = false;
        if (prev::scene::component::ComponentRepository<prev_test::component::ray_casting::ISelectableComponent>::Instance().Contains(node->GetId())) {
            selected = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::ISelectableComponent>::Instance().Get(node->GetId())->IsSelected();
        }

        // common
        uniformsFS.fogColor = prev_test::component::sky::FOG_COLOR;
        uniformsFS.selectedColor = prev_test::component::ray_casting::SELECTED_COLOR;
        uniformsFS.selected = selected;
        uniformsFS.castedByShadows = nodeRenderComponent->IsCastedByShadows();
        uniformsFS.heightScale = material->GetHeightScale();
        uniformsFS.numLayers = 16;

        uboFS->Update(&uniformsFS);

        m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), *shadowsComponent->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
        m_shader->Bind("colorSampler", material->GetImageBuffer()->GetImageView(), *material->GetImageSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        if (material->GetNormalmageBuffer()) {
            m_shader->Bind("normalSampler", material->GetNormalmageBuffer()->GetImageView(), *material->GetNormalImageSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        if (material->GetHeightImageBuffer()) {
            m_shader->Bind("heightSampler", material->GetHeightImageBuffer()->GetImageView(), *material->GetHeightImageSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
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
} // namespace prev_test::render::renderer::normal
