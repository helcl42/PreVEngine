#include "BoundingVolumeDebugRenderer.h"

#ifdef RENDER_BOUNDING_VOLUMES

#include "pipeline/BoundingVolumeDebugPipeline.h"
#include "shader/BoundingVolumeDebugShader.h"

#include "../../../component/ray_casting/IBoundingVolumeComponent.h"
#include "../../../component/ray_casting/RayCastingCommon.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::debug {
BoundingVolumeDebugRenderer::BoundingVolumeDebugRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void BoundingVolumeDebugRenderer::Init()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto allocator = prev::core::AllocatorProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::BoundingVolumeDebugShader>(*device, shader::BoundingVolumeDebugShader::GetPaths());
    m_shader->Init();
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Bounding Volume Debug Shader created\n");

    m_pipeline = std::make_unique<pipeline::BoundingVolumeDebugPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("Bounding Volume Debug Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsVS>>(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsFS>>(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));
}

void BoundingVolumeDebugRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
}

void BoundingVolumeDebugRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void BoundingVolumeDebugRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_BOUNDING_VOLUME_COMPONENT })) {
        const auto boundingVolumeComponent = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IBoundingVolumeComponent>::Instance().Get(node->GetId());

        auto uboVS = m_uniformsPoolVS->GetNext();

        UniformsVS uniformsVS{};
        uniformsVS.projectionMatrix = renderContext.projectionMatrix;
        uniformsVS.viewMatrix = renderContext.viewMatrix;
        uniformsVS.modelMatrix = glm::mat4(1.0f);

        uboVS->Update(&uniformsVS);

        auto uboFS = m_uniformsPoolFS->GetNext();

        UniformsFS uniformsFS{};
        uniformsFS.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
        uniformsFS.selectedColor = prev_test::component::ray_casting::SELECTED_COLOR;
        uniformsFS.selected = false;

        uboFS->Update(&uniformsFS);

        m_shader->Bind("uboVS", *uboVS);
        m_shader->Bind("uboFS", *uboFS);

        const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *boundingVolumeComponent->GetModel()->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *boundingVolumeComponent->GetModel()->GetIndexBuffer(), 0, boundingVolumeComponent->GetModel()->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, boundingVolumeComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }
}

void BoundingVolumeDebugRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void BoundingVolumeDebugRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void BoundingVolumeDebugRenderer::ShutDown()
{
    m_pipeline->ShutDown();
    m_pipeline = nullptr;

    m_shader->ShutDown();
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::debug

#endif