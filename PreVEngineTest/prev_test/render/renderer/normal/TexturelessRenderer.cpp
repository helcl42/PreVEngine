#include "TexturelessRenderer.h"
#include "pipeline/TexturelessPipeline.h"
#include "shader/TexturelessShader.h"

#include "../../../component/light/ILightComponent.h"
#include "../../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../../component/ray_casting/ISelectableComponent.h"
#include "../../../component/ray_casting/RayCastingCommon.h"
#include "../../../component/render/IRenderComponent.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/sky/SkyCommon.h"
#include "../../../component/transform/ITransformComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::normal {
TexturelessRenderer::TexturelessRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void TexturelessRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::TexturelessShader>(*device, shader::TexturelessShader::GetPaths());
    m_shader->Init();
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Textureless Shader created\n");

    m_pipeline = std::make_unique<pipeline::TexturelessPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("Textureless Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsVS>>(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsFS>>(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));
}

void TexturelessRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void TexturelessRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void TexturelessRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_RENDER_TEXTURELESS_COMPONENT, TAG_TRANSFORM_COMPONENT })) {
        bool visible{ true };
        if (prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
            visible = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContext.frustum);
        }

        if (visible) {
            const auto nodeRenderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IRenderComponent>::Instance().Get(node->GetId());
            RenderMeshNode(renderContext, node, nodeRenderComponent->GetModel()->GetMesh()->GetRootNode());
        }
    }
}

void TexturelessRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void TexturelessRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void TexturelessRenderer::ShutDown()
{
    m_pipeline->ShutDown();
    m_pipeline = nullptr;

    m_shader->ShutDown();
    m_shader = nullptr;
}

void TexturelessRenderer::RenderMeshNode(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const prev_test::render::MeshNode& meshNode)
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
        const auto& meshPart = meshParts[meshPartIndex];
        const auto material = nodeRenderComponent->GetMaterial(meshPart.materialIndex);
        const auto modelMatrix = transformComponent->GetWorldTransformScaled() * meshNode.transform;

        auto uboVS = m_uniformsPoolVS->GetNext();

        UniformsVS uniformsVS{};
        uniformsVS.projectionMatrix = renderContext.projectionMatrix;
        uniformsVS.viewMatrix = renderContext.viewMatrix;
        uniformsVS.modelMatrix = modelMatrix;
        uniformsVS.normalMatrix = glm::inverse(modelMatrix);
        uniformsVS.textureNumberOfRows = material->GetAtlasNumberOfRows();
        uniformsVS.textureOffset = glm::vec4(material->GetTextureOffset(), 0.0f, 0.0f);
        uniformsVS.cameraPosition = glm::vec4(renderContext.cameraPosition, 1.0f);
        for (size_t i = 0; i < lightComponents.size(); i++) {
            const auto& lightComponent{ lightComponents[i] };
            uniformsVS.lightning.lights[i] = LightUniform(glm::vec4(lightComponent->GetPosition(), 1.0f), glm::vec4(lightComponent->GetColor(), 1.0f), glm::vec4(lightComponent->GetAttenuation(), 1.0f));
        }
        uniformsVS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
        uniformsVS.lightning.ambientFactor = prev_test::component::light::AMBIENT_LIGHT_INTENSITY;
        uniformsVS.useFakeLightning = material->UsesFakeLightning();
        uniformsVS.density = prev_test::component::sky::FOG_DENSITY;
        uniformsVS.gradient = prev_test::component::sky::FOG_GRADIENT;
        uniformsVS.clipPlane = renderContext.clipPlane;

        uboVS->Update(&uniformsVS);

        auto uboFS = m_uniformsPoolFS->GetNext();

        UniformsFS uniformsFS{};
        // shadows
        for (uint32_t i = 0; i < prev_test::component::shadow::CASCADES_COUNT; i++) {
            const auto& cascade{ shadowsComponent->GetCascade(i) };
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

        uboFS->Update(&uniformsFS);

        m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), *shadowsComponent->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
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
} // namespace prev_test::render::renderer::normal
