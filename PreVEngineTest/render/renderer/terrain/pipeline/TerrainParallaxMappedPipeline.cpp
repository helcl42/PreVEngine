#include "TerrainParallaxMappedPipeline.h"

#include "../../../pipeline/PipelineFactory.h"

namespace prev_test::render::renderer::terrain::pipeline {
TerrainParallaxMappedPipeline::TerrainParallaxMappedPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass)
    : AbstractGraphicsPipeline(device, shaders, renderPass)
{
}

VkPipeline TerrainParallaxMappedPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateDefaultPipeline(m_device, m_shaders, m_renderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
    return m_pipeline;
}
} // namespace prev_test::render::renderer::terrain::pipeline
