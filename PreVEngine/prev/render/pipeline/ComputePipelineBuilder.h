#ifndef __COMPUTE_PIPELINE_BUILDER_H__
#define __COMPUTE_PIPELINE_BUILDER_H__

#include "AbstractPipelineBuilder.h"

namespace prev::render::pipeline {
class ComputePipelineBuilder final : public AbstractPipelineBuilder {
public:
    ComputePipelineBuilder(const VkDevice device, const shader::Shader& shader);

    ~ComputePipelineBuilder() = default;

public:
    std::unique_ptr<Pipeline> Build() const override;

private:
    void Validate() const override;

private:
    VkPipeline CreateComputePipeline(const VkPipelineLayout pipelineLayout) const;
};
} // namespace prev::render::pipeline

#endif