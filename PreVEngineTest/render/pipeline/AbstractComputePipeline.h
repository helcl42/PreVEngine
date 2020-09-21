#ifndef __ABSTRACT_COMPUTE_PIPELINE_H__
#define __ABSTRACT_COMPUTE_PIPELINE_H__

#include "AbstractPipeline.h"

namespace prev_test::render::pipeline {
class AbstractComputePipeline : public AbstractPipeline {
protected:
    AbstractComputePipeline(const VkDevice device, const prev::render::shader::Shader& shaders)
        : AbstractPipeline(device, shaders)
    {
    }

    ~AbstractComputePipeline() = default;
};
} // namespace prev_test::render::pipeline

#endif