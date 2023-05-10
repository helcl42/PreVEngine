#include "TerrainConeStepMappedPipeline.h"

#include "../../../pipeline/PipelineFactory.h"

namespace prev_test::render::renderer::terrain::pipeline {
TerrainConeStepMappedPipeline::TerrainConeStepMappedPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass)
    : AbstractGraphicsPipeline(device, shaders, renderPass)
{
}

bool TerrainConeStepMappedPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateDefaultPipeline(m_device, m_shaders, m_renderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, false, true, 1, m_pipelineLayout, m_pipeline);
    return m_pipeline != VK_NULL_HANDLE;
}
} // namespace prev_test::render::renderer::terrain::pipeline
