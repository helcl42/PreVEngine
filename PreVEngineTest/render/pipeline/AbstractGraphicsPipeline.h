#ifndef __ABSTRACT_GRAPHICS_PIPELINE_H__
#define __ABSTRACT_GRAPHICS_PIPELINE_H__

#include "AbstractPipeline.h"

#include <prev/render/pass/RenderPass.h>

namespace prev_test::render::pipeline {
class AbstractGraphicsPipeline : public AbstractPipeline {
protected:
    AbstractGraphicsPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass, const VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT)
        : AbstractPipeline(device, shaders)
        , m_renderPass(renderPass)
        , m_samplesCount(sampleCount)
    {
    }

    virtual ~AbstractGraphicsPipeline() = default;

protected:
    const prev::render::pass::RenderPass m_renderPass;

    const VkSampleCountFlagBits m_samplesCount;
};
} // namespace prev_test::render::pipeline

#endif // !__ABSTRACT_GRAPHICS_PIPELINE_H__
