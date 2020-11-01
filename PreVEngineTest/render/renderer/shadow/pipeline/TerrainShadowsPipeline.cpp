#include "TerrainShadowsPipeline.h"

#include "../../../pipeline/PipelineFactory.h"

namespace prev_test::render::renderer::shadow::pipeline {
TerrainShadowsPipeline::TerrainShadowsPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const VkRenderPass renderpass, const VkSampleCountFlagBits sampleCount)
    : AbstractGraphicsPipeline(device, shaders, renderpass, sampleCount)
{
}

VkPipeline TerrainShadowsPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateShadowsPipeline(m_device, m_shaders, m_renderPass, m_samplesCount, m_pipelineLayout, m_pipeline);
    return m_pipeline;
}

} // namespace prev_test::render::renderer::shadow::pipeline
