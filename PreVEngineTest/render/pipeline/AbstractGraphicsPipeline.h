#ifndef __ABSTRACT_GRAPHICS_PIPELINE_H__
#define __ABSTRACT_GRAPHICS_PIPELINE_H__

#include "AbstractPipeline.h"

namespace prev_test::render::pipeline {
class AbstractGraphicsPipeline : public AbstractPipeline {
protected:
    AbstractGraphicsPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders)
        : AbstractPipeline(device, shaders)
        , m_renderPass(renderpass)
    {
    }

    virtual ~AbstractGraphicsPipeline() = default;

protected:
    const VkRenderPass m_renderPass;
};
} // namespace prev_test::render::pipeline

#endif // !__ABSTRACT_GRAPHICS_PIPELINE_H__
