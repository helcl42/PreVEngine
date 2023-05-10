#include "SelectionDebugPipeline.h"

#include "../../../pipeline/PipelineFactory.h"

namespace prev_test::render::renderer::debug::pipeline {
SelectionDebugPipeline::SelectionDebugPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass)
    : AbstractGraphicsPipeline(device, shaders, renderPass)
{
}

bool SelectionDebugPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateDefaultPipeline(m_device, m_shaders, m_renderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, false, false, 1, m_pipelineLayout, m_pipeline);
    return m_pipeline != VK_NULL_HANDLE;
}
} // namespace prev_test::render::renderer::debug::pipeline
