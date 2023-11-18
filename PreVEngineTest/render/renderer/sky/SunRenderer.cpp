#include "SunRenderer.h"
#include "pipeline/SunOcclusionPipeline.h"
#include "shader/SunOcclusionShader.h"

#include "SkyEvents.h"

#include "../../../component/sky/ISunComponent.h"

#include <prev/core/AllocatorProvider.h>
#include <prev/core/DeviceProvider.h>
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
    m_shader->Init();
    m_shader->AdjustDescriptorPoolCapacity(m_descriptorCount);

    LOGI("Sun Shader created\n");

    m_pipeline = std::make_unique<pipeline::SunOcclusionPipeline>(*device, *m_shader, *m_renderPass);
    m_pipeline->Init();

    LOGI("Sun Pipeline created\n");

    m_uniformsPoolVS = std::make_unique<prev::render::buffer::UniformBufferRing<UniformsVS>>(*allocator);
    m_uniformsPoolVS->AdjustCapactity(m_descriptorCount, static_cast<uint32_t>(device->GetGPU()->GetProperties().limits.minUniformBufferOffsetAlignment));

    VkQueryPoolCreateInfo queryPoolInfo = { VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO };
    queryPoolInfo.queryType = VK_QUERY_TYPE_OCCLUSION;
    queryPoolInfo.queryCount = 1;
    VKERRCHECK(vkCreateQueryPool(*device, &queryPoolInfo, nullptr, &m_queryPool));

    m_firstFrame = true;
    m_passedSamples = 0;
}

void SunRenderer::BeforeRender(const NormalRenderContext& renderContext)
{
    if (!m_firstFrame) {
        auto device = prev::core::DeviceProvider::Instance().GetDevice();

        const auto result{ vkGetQueryPoolResults(*device, m_queryPool, 0, 1, sizeof(m_passedSamples), &m_passedSamples, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_PARTIAL_BIT) };
        const float ratio{ glm::clamp((static_cast<float>(m_passedSamples) / static_cast<float>(m_maxNumberOfSamples * m_renderPass->GetSamplesCount())) * 0.5f, 0.0f, 1.0f) };

        prev::event::EventChannel::Post(prev_test::render::renderer::sky::SunVisibilityEvent{ ratio });
    }
    m_firstFrame = false;

    vkCmdResetQueryPool(renderContext.commandBuffer, m_queryPool, 0, 1);
}

void SunRenderer::PreRender(const NormalRenderContext& renderContext)
{
    const VkRect2D scissor{ { renderContext.rect.offset.x, renderContext.rect.offset.y }, { renderContext.rect.extent.width, renderContext.rect.extent.height } };
    const VkViewport viewport{ static_cast<float>(renderContext.rect.offset.x), static_cast<float>(renderContext.rect.offset.y), static_cast<float>(renderContext.rect.extent.width), static_cast<float>(renderContext.rect.extent.height), 0, 1 };

    vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
    vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
}

void SunRenderer::Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node)
{
    if (node->GetTags().HasAll({ TAG_SUN_RENDER_COMPONENT })) {
        const auto sunComponent{ prev::scene::component::ComponentRepository<prev_test::component::sky::ISunComponent>::Instance().Get(node->GetId()) };

        const float aspectRatio{ static_cast<float>(renderContext.rect.extent.width - renderContext.rect.offset.x) / static_cast<float>(renderContext.rect.extent.height - renderContext.rect.offset.y) };
        const float xScale{ sunComponent->GetFlare()->GetScale() };
        const float yScale{ xScale * aspectRatio };

        m_maxNumberOfSamples = static_cast<uint64_t>(xScale * static_cast<float>(renderContext.rect.extent.width - renderContext.rect.offset.x) * yScale * static_cast<float>(renderContext.rect.extent.height - renderContext.rect.offset.y));

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
}

void SunRenderer::PostRender(const NormalRenderContext& renderContext)
{
}

void SunRenderer::AfterRender(const NormalRenderContext& renderContext)
{
}

void SunRenderer::ShutDown()
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();
    vkDestroyQueryPool(*device, m_queryPool, nullptr);

    m_pipeline->ShutDown();
    m_pipeline = nullptr;

    m_shader->ShutDown();
    m_shader = nullptr;
}
} // namespace prev_test::render::renderer::sky
