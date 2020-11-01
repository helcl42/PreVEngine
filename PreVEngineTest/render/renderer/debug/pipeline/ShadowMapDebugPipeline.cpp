#include "ShadowMapDebugPipeline.h"

#include "../../../pipeline/PipelineFactory.h"

namespace prev_test::render::renderer::debug::pipeline {
ShadowMapDebugPipeline::ShadowMapDebugPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const VkRenderPass renderpass, const VkSampleCountFlagBits sampleCount)
    : AbstractGraphicsPipeline(device, shaders, renderpass, sampleCount)
{
}

VkPipeline ShadowMapDebugPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateDebugPipeline(m_device, m_shaders, m_renderPass, m_samplesCount, m_pipelineLayout, m_pipeline);
    return m_pipeline;
}
} // namespace prev_test::render::renderer::debug::pipeline
