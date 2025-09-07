#include "ComputePipelineBuilder.h"

namespace prev::render::pipeline {
ComputePipelineBuilder::ComputePipelineBuilder(const VkDevice device, const shader::Shader& shader)
    : AbstractPipelineBuilder(device, shader)
{
}

std::unique_ptr<Pipeline> ComputePipelineBuilder::Build() const
{
    Validate();

    auto pipelineLayout{ CreatePipelineLayout() };
    auto pipeline{ CreateComputePipeline(pipelineLayout) };

    return std::unique_ptr<Pipeline>(new Pipeline(m_device, pipeline, pipelineLayout));
}

void ComputePipelineBuilder::Validate() const
{
    if (m_shader.GetShaderStages().size() != 1) {
        throw std::runtime_error("Invalid pipeline configuration:Shader with shader stages count != 1 seems to be incompatible.");
    }
}

VkPipeline ComputePipelineBuilder::CreateComputePipeline(const VkPipelineLayout pipelineLayout) const
{
    VkComputePipelineCreateInfo computePipelineCreateInfo{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
    computePipelineCreateInfo.layout = pipelineLayout;
    computePipelineCreateInfo.flags = 0;
    computePipelineCreateInfo.stage = m_shader.GetShaderStages().at(0);

    VkPipeline pipeline;
    VKERRCHECK(vkCreateComputePipelines(m_device, nullptr, 1, &computePipelineCreateInfo, nullptr, &pipeline));
    return pipeline;
}
} // namespace prev::render::pipeline