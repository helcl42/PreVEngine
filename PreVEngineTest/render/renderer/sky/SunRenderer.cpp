#include "SunRenderer.h"
#include "pipeline/SunOcclusionPipeline.h"
#include "shader/SunOcclusionShader.h"

#include "SkyEvents.h"

#include "../../../component/sky/ISunComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/event/EventsChannel.h>
#include <prev/render/shader/ShaderFactory.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::render::renderer::sky {
SunRenderer::SunRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass)
    : m_renderPass(renderPass)
{
}

void SunRenderer::Init()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::core::AllocatorProvider::Instance().GetAllocator() };

    prev::render::shader::ShaderFactory shaderFactory;
    m_shader = shaderFactory.CreateShaderFromFiles<shader::SunOcclusionShader>(*device, shader::SunOcclusionShader::GetPaths());
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Sun Shader created\n");

    m_pipeline = std::make_unique<pipeline::SunOcclusionPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("Sun Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::core::memory::buffer::UBOPool<UniformsVS> >(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    VkQueryPoolCreateInfo queryPoolInfo = { VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO };
    queryPoolInfo.queryType = VK_QUERY_TYPE_OCCLUSION;
    queryPoolInfo.queryCount = 1;
    VKERRCHECK(vkCreateQueryPool(*device, &queryPoolInfo, nullptr, &m_queryPool));
}

void SunRenderer::BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    m_passedSamples = 0;
    vkCmdResetQueryPool(renderContext.commandBuffer, m_queryPool, 0, 1);
}

void SunRenderer::PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    const VkRect2D scissor{ { 0, 0 }, renderContext.fullExtent };
    const VkViewport viewport{ 0, 0, static_cast<float>(renderContext.fullExtent.width), static_cast<float>(renderContext.fullExtent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void SunRenderer::Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const NormalRenderContextUserData& renderContextUserData)
{
    if (node->GetTags().HasAll({ TAG_SUN_RENDER_COMPONENT })) {
        const auto sunComponent = prev::scene::component::ComponentRepository<prev_test::component::sky::ISunComponent>::Instance().Get(node->GetId());

        const float xScale = sunComponent->GetFlare()->GetScale();
        const float yScale = xScale * renderContext.fullExtent.width / renderContext.fullExtent.height;

        m_maxNumberOfSamples = static_cast<uint64_t>(powf(sunComponent->GetFlare()->GetScale() * renderContext.fullExtent.width, 2.0f));

        vkCmdBeginQuery(renderContext.commandBuffer, m_queryPool, 0, 0);

        auto uboVS = m_uniformsPoolVS->GetNext();
        UniformsVS uniformsVS{};
        uniformsVS.translation = glm::vec4(sunComponent->GetFlare()->GetScreenSpacePosition(), 0.0f, 1.0f);
        uniformsVS.scale = glm::vec4(xScale, yScale, 0.0f, 0.0f);
        uboVS->Update(&uniformsVS);

        m_shader->Bind("uboVS", *uboVS);

        const VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
        const VkBuffer vertexBuffers[] = { *sunComponent->GetModel()->GetVertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(renderContext.commandBuffer, *sunComponent->GetModel()->GetIndexBuffer(), 0, sunComponent->GetModel()->GetIndexBuffer()->GetIndexType());
        vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(renderContext.commandBuffer, sunComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);

        vkCmdEndQuery(renderContext.commandBuffer, m_queryPool, 0);
    }

    for (auto child : node->GetChildren()) {
        Render(renderContext, child, renderContextUserData);
    }
}

void SunRenderer::PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
}

void SunRenderer::AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData)
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();

    //#if defined(__ANDROID__)
    auto result = vkGetQueryPoolResults(*device, m_queryPool, 0, 1, sizeof(m_passedSamples), &m_passedSamples, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_PARTIAL_BIT);
    //#else
    //        auto result = vkGetQueryPoolResults(*device, m_queryPool, 0, 1, sizeof(m_passedSamples), &m_passedSamples, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);
    //#endif
    const float ratio = glm::clamp((static_cast<float>(m_passedSamples) / static_cast<float>(m_maxNumberOfSamples * m_renderPass->GetSamplesCount())), 0.0f, 1.0f) * 1.2f;
    //LOGI("Result: %s Passed samples: %lld Max: %lld Ratio: %f\n", VkResultStr(result),  m_passedSamples,  m_maxNumberOfSamples, ratio);
    prev::event::EventChannel::Post(prev_test::render::renderer::sky::SunVisibilityEvent{ ratio });
}

void SunRenderer::ShutDown()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    vkDestroyQueryPool(*device, m_queryPool, nullptr);

    m_shader->ShutDown();

    m_pipeline->ShutDown();
}
} // namespace prev_test::render::renderer::sky
