#include "ShadowMapDebugPipeline.h"

#include "../../pipeline/PipelineFactory.h"

namespace prev_test::render::debug::pipeline {
ShadowMapDebugPipeline::ShadowMapDebugPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders)
    : AbstractGraphicsPipeline(device, renderpass, shaders)
{
}

VkPipeline ShadowMapDebugPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateDebugPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_pipeline);
    return m_pipeline;
}
} // namespace prev_test::render::debug::pipeline