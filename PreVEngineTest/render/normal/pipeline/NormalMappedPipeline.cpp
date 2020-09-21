#include "NormalMappedPipeline.h"

#include "../../pipeline/PipelineFactory.h"

namespace prev_test::render::normal::pipeline {
NormalMappedPipeline::NormalMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders)
    : AbstractGraphicsPipeline(device, renderpass, shaders)
{
}

VkPipeline NormalMappedPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
    return m_pipeline;
}
} // namespace prev_test::render::normal::pipeline