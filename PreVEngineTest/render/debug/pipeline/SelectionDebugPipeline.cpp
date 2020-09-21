#include "SelectionDebugPipeline.h"

#include "../../pipeline/PipelineFactory.h"

namespace prev_test::render::debug::pipeline {
SelectionDebugPipeline::SelectionDebugPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders)
    : AbstractGraphicsPipeline(device, renderpass, shaders)
{
}

VkPipeline SelectionDebugPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, false, m_pipelineLayout, m_pipeline);
    return m_pipeline;
}
} // namespace prev_test::render::debug::pipeline