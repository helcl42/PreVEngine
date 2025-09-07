#include "AbstractPipelineBuilder.h"

namespace prev::render::pipeline {
AbstractPipelineBuilder::AbstractPipelineBuilder(const VkDevice device, const shader::Shader& shader)
    : m_device{ device }
    , m_shader{ shader }
{
}

VkPipelineLayout AbstractPipelineBuilder::CreatePipelineLayout() const
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_shader.GetDescriptorSetLayout();
    pipelineLayoutInfo.pPushConstantRanges = m_shader.GetPushConstantsRanges().data();
    pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(m_shader.GetPushConstantsRanges().size());

    VkPipelineLayout pipelineLayout;
    VKERRCHECK(vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &pipelineLayout));
    return pipelineLayout;
}
} // namespace prev::render::pipeline