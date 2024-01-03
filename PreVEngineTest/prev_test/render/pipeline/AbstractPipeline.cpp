#include "AbstractPipeline.h"

namespace prev_test::render::pipeline {
void AbstractPipeline::ShutDown()
{
    if (m_device) {
        vkDeviceWaitIdle(m_device);
    }

    if (m_pipelineLayout) {
        vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }

    if (m_pipeline) {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }
}

VkPipelineLayout AbstractPipeline::GetLayout() const
{
    return m_pipelineLayout;
}

AbstractPipeline::operator VkPipeline() const
{
    return m_pipeline;
}

AbstractPipeline::AbstractPipeline(const VkDevice device, const prev::render::shader::Shader& shaders)
    : m_device(device)
    , m_shaders(shaders)
    , m_pipeline(VK_NULL_HANDLE)
    , m_pipelineLayout(VK_NULL_HANDLE)
{
}
} // namespace prev_test::render::pipeline