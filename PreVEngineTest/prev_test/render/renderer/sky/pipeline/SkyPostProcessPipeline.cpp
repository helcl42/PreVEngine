#include "SkyPostProcessPipeline.h"

#include "../../../pipeline/PipelineFactory.h"

namespace prev_test::render::renderer::sky::pipeline {
SkyPostProcessPipeline::SkyPostProcessPipeline(const VkDevice device, const prev::render::shader::Shader& shaders)
    : AbstractComputePipeline(device, shaders)
{
}

bool SkyPostProcessPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    m_pipelineLayout = pipelineFactory.CreatePipelineLayout(m_device, m_shaders);
    m_pipeline = pipelineFactory.CreateDefaultComputePipeline(m_device, m_shaders, m_pipelineLayout);
    return m_pipeline != VK_NULL_HANDLE;
}
} // namespace prev_test::render::renderer::sky::pipeline
