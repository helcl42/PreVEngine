#include "NormalMappedRenderer.h"

#include "pipeline/NormalMappedPipeline.h"
#include "shader/NormalMappedShader.h"

#include "../../../common/AssetManager.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/render/IRenderComponent.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/sky/SkyCommon.h"
#include "../../../component/transform/ITransformComponent.h"

#include "../../../RayCasting.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/AllocatorProvider.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

#include <memory>

namespace prev_test::render::renderer::normal {
NormalMappedRenderer::NormalMappedRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void NormalMappedRenderer::Init()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<prev_test::render::renderer::normal::shader::NormalMappedShader>(*device, { { VK_SHADER_STAGE_VERTEX_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/normal_mapped_vert.spv") }, { VK_SHADER_STAGE_FRAGMENT_BIT, prev_test::common::AssetManager::Instance().GetAssetPath("Shaders/normal_mapped_frag.spv") } });
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Normal Mapped Shader created\n");

    m_pipeline = std::make_unique<prev_test::render::renderer::normal::pipeline::NormalMappedPipeline>(*device, *m_renderPass, *m_shader);
    m_pipeline->Init();

    LOGI("Normal Mapped Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsVS> >(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsFS> >(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
}

void NormalMappedRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void NormalMappedRenderer::PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { 0, 0 }, renderContext.fullExtent };
    const VkViewport viewport{ 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void NormalMappedRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData)
{
    if (node->GetFlags().HasAll(prev::common::FlagSet<SceneNodeFlags>{ SceneNodeFlags::RENDER_NORMAL_MAPPED_COMPONENT | SceneNodeFlags::TRANSFORM_COMPONENT })) {
        bool visible = true;
        if (prev::scene::component::ComponentRepository<IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
            visible = prev::scene::component::ComponentRepository<IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContextUserData.frustum);
        }

        if (visible) {
            const auto mainLightComponent = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT });
            const auto shadowsComponent = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, prev_test::component::shadow::IShadowsComponent>({ TAG_SHADOW });
            const auto lightComponents = prev::scene::component::NodeComponentHelper::FindAll<SceneNodeFlags, prev_test::component::light::ILightComponent>({ TAG_LIGHT });

            const auto transformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(node->GetId());
            const auto nodeRenderComponent = prev::scene::component::ComponentRepository<prev_test::component::render::IRenderComponent>::Instance().Get(node->GetId());

            const auto vertexStride = nodeRenderComponent->GetModel()->GetMesh()->GetVertexLayout().GetStride();

            const auto meshParts = nodeRenderComponent->GetModel()->GetMesh()->GetMeshParts();
            for (const auto& meshPart : meshParts) {
                const auto modelMatrix = transformComponent->GetWorldTransformScaled() * meshPart.transform;

                auto uboVS = m_uniformsPoolVS->GetNext();

                UniformsVS uniformsVS{};
                uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
                uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
                uniformsVS.modelMatrix = modelMatrix;
                uniformsVS.normalMatrix = glm::inverse(modelMatrix);
                uniformsVS.textureNumberOfRows = nodeRenderComponent->GetMaterial()->GetAtlasNumberOfRows();
                uniformsVS.textureOffset = glm::vec4(nodeRenderComponent->GetMaterial()->GetTextureOffset(), 0.0f, 0.0f);
                uniformsVS.cameraPosition = glm::vec4(renderContextUserData.cameraPosition, 1.0f);
                for (size_t i = 0; i < lightComponents.size(); i++) {
                    uniformsVS.lightning.lights[i] = LightUniform(glm::vec4(lightComponents[i]->GetPosition(), 1.0f), glm::vec4(lightComponents[i]->GetColor(), 1.0f), glm::vec4(lightComponents[i]->GetAttenuation(), 1.0f));
                }
                uniformsVS.lightning.realCountOfLights = static_cast<uint32_t>(lightComponents.size());
                uniformsVS.lightning.ambientFactor = prev_test::component::light::AMBIENT_LIGHT_INTENSITY;
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
                uniformsFS.material = MaterialUniform(nodeRenderComponent->GetMaterial()->GetShineDamper(), nodeRenderComponent->GetMaterial()->GetReflectivity());

                bool selected = false;
                if (prev::scene::component::ComponentRepository<ISelectableComponent>::Instance().Contains(node->GetId())) {
                    selected = prev::scene::component::ComponentRepository<ISelectableComponent>::Instance().Get(node->GetId())->IsSelected();
                }

                // common
                uniformsFS.fogColor = prev_test::component::sky::FOG_COLOR;
                uniformsFS.selectedColor = SELECTED_COLOR;
                uniformsFS.selected = selected;
                uniformsFS.castedByShadows = nodeRenderComponent->IsCastedByShadows();

                uboFS->Update(&uniformsFS);

                m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), shadowsComponent->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
                m_shader->Bind("textureSampler", *nodeRenderComponent->GetMaterial(meshPart.materialIndex)->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("normalSampler", *nodeRenderComponent->GetMaterial(meshPart.materialIndex)->GetNormalmageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_shader->Bind("uboVS", *uboVS);
                m_shader->Bind("uboFS", *uboFS);

                const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
                const VkBuffer vertexBuffers[] = { *nodeRenderComponent->GetModel()->GetVertexBuffer() };
                const VkDeviceSize offsets[] = { meshPart.firstVertexIndex * vertexStride };

                vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(renderContext.commandBuffer, *nodeRenderComponent->GetModel()->GetIndexBuffer(), 0, nodeRenderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
                vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

                vkCmdDrawIndexed(renderContext.commandBuffer, meshPart.indicesCount, 1, meshPart.firstIndicesIndex, 0, 0);
            }
        }
    }

    for (auto child : node->GetChildren()) {
        Render(renderContext, child, renderContextUserData);
    }
}

void NormalMappedRenderer::PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void NormalMappedRenderer::AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void NormalMappedRenderer::ShutDown()
{
    m_shader->ShutDown();

    m_pipeline->ShutDown();
}
} // namespace prev_test::render::renderer::normal
