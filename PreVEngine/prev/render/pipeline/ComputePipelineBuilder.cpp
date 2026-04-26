#include "ComputePipelineBuilder.h"

#include "../../common/Logger.h"

#include <stdexcept>

namespace prev::render::pipeline {
ComputePipelineBuilder::ComputePipelineBuilder(GfxDevice device, const shader::Shader& shader)
    : AbstractPipelineBuilder(device, shader)
{
}

std::unique_ptr<Pipeline> ComputePipelineBuilder::Build() const
{
    Validate();

    auto pipeline{ CreateComputePipeline() };

    return std::unique_ptr<Pipeline>(new Pipeline(m_device, pipeline));
}

void ComputePipelineBuilder::Validate() const
{
    if (m_shader.GetShaderModules().size() != 1) {
        throw std::runtime_error("Invalid pipeline configuration: Shader with shader stages count != 1 seems to be incompatible.");
    }
}

GfxComputePipeline ComputePipelineBuilder::CreateComputePipeline() const
{
    const auto& modules{ m_shader.GetShaderModules() };
    const auto& [stage, gfxShader] = *modules.begin();

    GfxBindGroupLayout bindGroupLayout{ m_shader.GetBindGroupLayout() };

    GfxComputePipelineDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_COMPUTE_PIPELINE_DESCRIPTOR;
    desc.compute = gfxShader;
    desc.entryPoint = "main";
    desc.bindGroupLayouts = &bindGroupLayout;
    desc.bindGroupLayoutCount = 1;

    GfxComputePipeline pipeline{};
    GFXERRCHECK(gfxDeviceCreateComputePipeline(m_device, &desc, &pipeline));
    return pipeline;
}
} // namespace prev::render::pipeline