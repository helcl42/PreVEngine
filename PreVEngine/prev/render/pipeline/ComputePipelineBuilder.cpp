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

    ValidateConstants(m_constants, "compute");
}

GfxComputePipeline ComputePipelineBuilder::CreateComputePipeline() const
{
    const auto& modules{ m_shader.GetShaderModules() };
    const auto& [stage, gfxShader] = *modules.begin();

    GfxBindGroupLayout bindGroupLayout{ m_shader.GetBindGroupLayout() };

    GfxComputeState computeState{};
    computeState.module = gfxShader;
    computeState.entryPoint = "computeMain";
    computeState.constants = m_constants.empty() ? nullptr : m_constants.data();
    computeState.constantCount = static_cast<uint32_t>(m_constants.size());

    GfxComputePipelineDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_COMPUTE_PIPELINE_DESCRIPTOR;
    desc.compute = &computeState;
    desc.bindGroupLayouts = &bindGroupLayout;
    desc.bindGroupLayoutCount = 1;

    GfxComputePipeline pipeline{};
    GFXERRCHECK(gfxDeviceCreateComputePipeline(m_device, &desc, &pipeline));
    return pipeline;
}
} // namespace prev::render::pipeline