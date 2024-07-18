#include "Pipeline.h"

namespace prev::render::pipeline {
Pipeline::Pipeline(const VkDevice device, const VkPipeline pipeline, const VkPipelineLayout pipelineLayout)
    : m_device{ device }
    , m_pipeline{ pipeline }
    , m_pipelineLayout{ pipelineLayout }
{
}

Pipeline::~Pipeline()
{
    vkDeviceWaitIdle(m_device);

    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    vkDestroyPipeline(m_device, m_pipeline, nullptr);
}

VkPipelineLayout Pipeline::GetLayout() const
{
    return m_pipelineLayout;
}

Pipeline::operator VkPipeline() const
{
    return m_pipeline;
}
} // namespace prev::render::pipeline