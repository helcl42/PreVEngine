#include "PerlinWorleyNoisePipeline.h"

#include "../../../pipeline/PipelineFactory.h"

namespace prev_test::render::renderer::sky::pipeline {
PerlinWorleyNoisePipeline::PerlinWorleyNoisePipeline(const VkDevice device, const prev::render::shader::Shader& shaders)
    : AbstractComputePipeline(device, shaders)
{
}

VkPipeline PerlinWorleyNoisePipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateDefaultComputePipeline(m_device, m_shaders, m_pipelineLayout, m_pipeline);
    return m_pipeline;
}
} // namespace prev_test::render::renderer::sky::pipeline
