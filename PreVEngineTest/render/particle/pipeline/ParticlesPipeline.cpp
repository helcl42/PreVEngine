#include "ParticlesPipeline.h"

#include "../../pipeline/PipelineFactory.h"

namespace prev_test::render::particle::pipeline {
ParticlesPipeline::ParticlesPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders)
    : AbstractGraphicsPipeline(device, renderpass, shaders)
{
}

VkPipeline ParticlesPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateParticlesPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, false, true, m_pipelineLayout, m_pipeline);
    return m_pipeline;
}
} // namespace prev_test::render::particle::pipeline