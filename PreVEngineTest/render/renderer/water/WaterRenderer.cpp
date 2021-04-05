#include "WaterRenderer.h"
#include "pipeline/WaterPipeline.h"
#include "shader/WaterShader.h"

#include "../../../component/common/IOffScreenRenderPassComponent.h"
#include "../../../component/light/ILightComponent.h"
#include "../../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../../component/shadow/IShadowsComponent.h"
#include "../../../component/sky/SkyCommon.h"
#include "../../../component/transform/ITransformComponent.h"
#include "../../../component/water/IWaterComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::water {
WaterRenderer::WaterRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void WaterRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::WaterShader>(*device, shader::WaterShader::GetPaths());
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Water Shader created\n");

    m_pipeline = std::make_unique<pipeline::WaterPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("Water Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsVS> >(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsFS> >(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));
}

void WaterRenderer::PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void WaterRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void WaterRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const NormalRenderContextUserData& renderContextUserData)
{
    if (node->GetTags().HasAll({ TAG_WATER_RENDER_COMPONENT, TAG_TRANSFORM_COMPONENT })) {
        bool visible{ true };
        if (prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Contains(node->GetId())) {
            visible = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Get(node->GetId())->IsInFrustum(renderContextUserData.frustum);
        }

        if (visible) {
            const auto waterComponent = prev::scene::component::ComponentRepository<prev_test::component::water::IWaterComponent>::Instance().Get(node->GetId());
            const auto waterReflectionComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>({ TAG_WATER_REFLECTION_RENDER_COMPONENT });
            const auto waterRefractionComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::common::IOffScreenRenderPassComponent>({ TAG_WATER_REFRACTION_RENDER_COMPONENT });
            const auto mainLightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT });
            const auto shadowsComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>({ TAG_SHADOW });

            const auto transformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(node->GetId());

            auto uboVS = m_uniformsPoolVS->GetNext();

            UniformsVS uniformsVS{};
            uniformsVS.projectionMatrix = renderContextUserData.projectionMatrix;
            uniformsVS.viewMatrix = renderContextUserData.viewMatrix;
            uniformsVS.modelMatrix = transformComponent->GetWorldTransformScaled();
            uniformsVS.cameraPosition = glm::vec4(renderContextUserData.cameraPosition, 1.0f);
            uniformsVS.density = prev_test::component::sky::FOG_DENSITY;
            uniformsVS.gradient = prev_test::component::sky::FOG_GRADIENT;

            uboVS->Update(&uniformsVS);

            auto uboFS = m_uniformsPoolFS->GetNext();

            UniformsFS uniformsFS{};
            uniformsFS.fogColor = prev_test::component::sky::FOG_COLOR;
            uniformsFS.waterColor = waterComponent->GetMaterial()->GetColor();
            uniformsFS.light.color = glm::vec4(mainLightComponent->GetColor(), 1.0f);
            uniformsFS.light.position = glm::vec4(mainLightComponent->GetPosition(), 1.0f);
            uniformsFS.nearFarClippingPlane = glm::vec4(renderContextUserData.nearFarClippingPlane, 0.0f, 0.0f);
            uniformsFS.moveFactor = waterComponent->GetMoveFactor();
            // shadows
            for (uint32_t i = 0; i < prev_test::component::shadow::CASCADES_COUNT; i++) {
                const auto& cascade = shadowsComponent->GetCascade(i);
                uniformsFS.shadows.cascades[i].split = glm::vec4(cascade.endSplitDepth);
                uniformsFS.shadows.cascades[i].viewProjectionMatrix = cascade.GetBiasedViewProjectionMatrix();
            }
            uniformsFS.shadows.enabled = prev_test::component::shadow::SHADOWS_ENABLED;

            uboFS->Update(&uniformsFS);

            m_shader->Bind("uboVS", *uboVS);
            m_shader->Bind("uboFS", *uboFS);
            m_shader->Bind("depthSampler", shadowsComponent->GetImageBuffer()->GetImageView(), shadowsComponent->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
            m_shader->Bind("reflectionTexture", waterReflectionComponent->GetColorImageBuffer()->GetImageView(), waterReflectionComponent->GetColorImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_shader->Bind("refractionTexture", waterRefractionComponent->GetColorImageBuffer()->GetImageView(), waterRefractionComponent->GetColorImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_shader->Bind("dudvMapTexture", waterComponent->GetMaterial()->GetImageBuffer()->GetImageView(), waterComponent->GetMaterial()->GetImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_shader->Bind("normalMapTexture", waterComponent->GetMaterial()->GetNormalmageBuffer()->GetImageView(), waterComponent->GetMaterial()->GetNormalmageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_shader->Bind("depthMapTexture", waterRefractionComponent->GetDepthImageBuffer()->GetImageView(), waterRefractionComponent->GetDepthImageBuffer()->GetSampler(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

            const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
            const VkBuffer vertexBuffers[] = { *waterComponent->GetModel()->GetVertexBuffer() };
            const VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(renderContext.commandBuffer, *waterComponent->GetModel()->GetIndexBuffer(), 0, waterComponent->GetModel()->GetIndexBuffer()->GetIndexType());
            vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(renderContext.commandBuffer, waterComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
        }
    }

    for (auto& child : node->GetChildren()) {
        Render(renderContext, child, renderContextUserData);
    }
}

void WaterRenderer::PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void WaterRenderer::AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void WaterRenderer::ShutDown()
{
    m_shader->ShutDown();

    m_pipeline->ShutDown();
}
} // namespace prev_test::render::renderer::water
