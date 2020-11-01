#include "FontPipeline.h"

#include "../../../pipeline/PipelineFactory.h"

namespace prev_test::render::renderer::font::pipeline {
FontPipeline::FontPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const VkRenderPass renderpass, const VkSampleCountFlagBits sampleCount)
    : AbstractGraphicsPipeline(device, shaders, renderpass, sampleCount)
{
}

VkPipeline FontPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateDefaultPipeline(m_device, m_shaders, m_renderPass, m_samplesCount, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, false, true, m_pipelineLayout, m_pipeline);
    return m_pipeline;
}
} // namespace prev_test::render::renderer::font::pipeline
