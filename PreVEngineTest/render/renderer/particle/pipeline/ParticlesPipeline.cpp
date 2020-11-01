#include "ParticlesPipeline.h"

#include "../../../pipeline/PipelineFactory.h"

namespace prev_test::render::renderer::particle::pipeline {
ParticlesPipeline::ParticlesPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass)
    : AbstractGraphicsPipeline(device, shaders, renderPass)
{
}

VkPipeline ParticlesPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateParticlesPipeline(m_device, m_shaders, m_renderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, false, true, m_pipelineLayout, m_pipeline);
    return m_pipeline;
}
} // namespace prev_test::render::renderer::particle::pipeline
