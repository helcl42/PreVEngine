#include "AnimatedShadowsPipeline.h"

#include "../../../pipeline/PipelineFactory.h"

namespace prev_test::render::renderer::shadow::pipeline {
AnimatedShadowsPipeline::AnimatedShadowsPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass)
    : AbstractGraphicsPipeline(device, shaders, renderPass)
{
}

bool AnimatedShadowsPipeline::Init()
{
    prev_test::render::pipeline::PipelineFactory pipelineFactory{};
    pipelineFactory.CreateShadowsPipeline(m_device, m_shaders, m_renderPass, m_pipelineLayout, m_pipeline);
    return m_pipeline != VK_NULL_HANDLE;
}
} // namespace prev_test::render::renderer::shadow::pipeline
