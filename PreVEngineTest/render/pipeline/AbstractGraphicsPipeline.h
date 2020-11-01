#ifndef __ABSTRACT_GRAPHICS_PIPELINE_H__
#define __ABSTRACT_GRAPHICS_PIPELINE_H__

#include "AbstractPipeline.h"

#include <prev/render/pass/RenderPass.h>

namespace prev_test::render::pipeline {
class AbstractGraphicsPipeline : public AbstractPipeline {
protected:
    AbstractGraphicsPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass)
        : AbstractPipeline(device, shaders)
        , m_renderPass(renderPass)
    {
    }

    virtual ~AbstractGraphicsPipeline() = default;

protected:
    const prev::render::pass::RenderPass& m_renderPass;
};
} // namespace prev_test::render::pipeline

#endif // !__ABSTRACT_GRAPHICS_PIPELINE_H__
