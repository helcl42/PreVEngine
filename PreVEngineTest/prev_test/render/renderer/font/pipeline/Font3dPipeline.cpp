#include "Font3dPipeline.h"

#include "../../../pipeline/PipelineFactory.h"

namespace prev_test::render::renderer::font::pipeline {
Font3dPipeline::Font3dPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass)
    : AbstractGraphicsPipeline(device, shaders, renderPass)
{
}

bool Font3dPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    m_pipelineLayout = pipelineFactory.CreatePipelineLayout(m_device, m_shaders);
    m_pipeline = pipelineFactory.CreateDefaultPipeline(m_device, m_shaders, m_renderPass, m_pipelineLayout, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, true);
    return m_pipeline != VK_NULL_HANDLE;
}
} // namespace prev_test::render::renderer::font::pipeline
