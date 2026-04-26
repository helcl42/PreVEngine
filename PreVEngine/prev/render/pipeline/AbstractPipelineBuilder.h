#ifndef __ABSTRACT_PIPELINE_BUILDER_H__
#define __ABSTRACT_PIPELINE_BUILDER_H__

#include "Pipeline.h"

#include "../shader/Shader.h"

namespace prev::render::pipeline {
class AbstractPipelineBuilder {
protected:
    AbstractPipelineBuilder(GfxDevice device, const shader::Shader& shader);

    virtual ~AbstractPipelineBuilder() = default;

public:
    virtual std::unique_ptr<Pipeline> Build() const = 0;

protected:
    virtual void Validate() const = 0;

protected:
    const GfxDevice m_device;

    const shader::Shader& m_shader;
};
} // namespace prev::render::pipeline

#endif