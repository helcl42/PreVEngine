#include "RayCastDebugRenderer.h"

#ifdef RENDER_RAYCASTS

#include "pipeline/RayCastDebugPipeline.h"
#include "shader/RayCastDebugShader.h"

#include "../../../component/ray_casting/IRayCasterComponent.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/AllocatorProvider.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::debug {
RayCastDebugRenderer::RayCastDebugRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void RayCastDebugRenderer::Init()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    auto allocator = prev::scene::AllocatorProvider::Instance().GetAllocator();

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::RayCastDebugShader>(*device, );
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("RayCast Debug Shader created\n");

    m_pipeline = std::make_unique<pipeline::RayCastDebugPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("RayCast Debug Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsVS> >(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolGS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsGS> >(*allocator);
    m_uniformsPoolGS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));

    m_uniformsPoolFS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsFS> >(*allocator);
    m_uniformsPoolFS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU().GetProperties().limits.minUniformBufferOffsetAlignment));
}

void RayCastDebugRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void RayCastDebugRenderer::PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { 0, 0 }, renderContext.fullExtent };
    const VkViewport viewport{ 0, 0, static_cast<float>(renderContextUserData.extent.width), static_cast<float>(renderContextUserData.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void RayCastDebugRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const NormalRenderContextUserData& renderContextUserData)
{
    if (node->GetTags().HasAll({ TAG_RAYCASTER_COMPONENT })) {
        const auto rayCastingComponent = prev::scene::component::ComponentRepository<prev_test::component::ray_casting::IRayCasterComponent>::Instance().Get(node->GetId());

        auto uboVS = m_uniformsPoolVS->GetNext();

        UniformsVS uniformsVS{};
        uniformsVS.color = glm::vec3(1.0, 0.0, 0.0);

        uboVS->Update(&uniformsVS);

        auto uboGS = m_uniformsPoolGS->GetNext();

        UniformsGS uniformsGS{};
        uniformsGS.projectionMatrix = renderContextUserData.projectionMatrix;
        uniformsGS.viewMatrix = renderContextUserData.viewMatrix;
        uniformsGS.modelMatrix = glm::mat4(1.0f);

        uboGS->Update(&uniformsGS);

        auto uboFS = m_uniformsPoolFS->GetNext();

        UniformsFS uniformsFS{};
        uniformsFS.alpha = 0.7f;

        uboFS->Update(&uniformsFS);

        m_shader->Bind("uboVS", *uboVS);
        m_shader->Bind("uboGS", *uboGS);
        m_shader->Bind("uboFS", *uboFS);

        const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *rayCastingComponent->GetModel()->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *rayCastingComponent->GetModel()->GetIndexBuffer(), 0, rayCastingComponent->GetModel()->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, rayCastingComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
    }

    for (auto child : node->GetChildren()) {
        Render(renderContext, child, renderContextUserData);
    }
}

void RayCastDebugRenderer::PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void RayCastDebugRenderer::AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void RayCastDebugRenderer::ShutDown()
{
    m_shader->ShutDown();

    m_pipeline->ShutDown();
}
} // namespace prev_test::render::renderer::debug

#endif