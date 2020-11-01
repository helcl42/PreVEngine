#include "SelectionDebugPipeline.h"

#include "../../../pipeline/PipelineFactory.h"

namespace prev_test::render::renderer::debug::pipeline {
SelectionDebugPipeline::SelectionDebugPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const VkRenderPass renderpass, const VkSampleCountFlagBits sampleCount)
    : AbstractGraphicsPipeline(device, shaders, renderpass, sampleCount)
{
}

VkPipeline SelectionDebugPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateDefaultPipeline(m_device, m_shaders, m_renderPass, m_samplesCount, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, false, m_pipelineLayout, m_pipeline);
    return m_pipeline;
}
} // namespace prev_test::render::renderer::debug::pipeline
