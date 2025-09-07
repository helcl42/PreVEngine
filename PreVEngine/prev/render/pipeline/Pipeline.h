#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "../../core/Core.h"

namespace prev::render::pipeline {
class GraphicsPipelineBuilder;
class ComputePipelineBuilder;

class Pipeline final {
private:
    Pipeline(const VkDevice device, const VkPipeline pipeline, const VkPipelineLayout pipelineLayout);

public:
    ~Pipeline();

public:
    VkPipelineLayout GetLayout() const;

public:
    operator VkPipeline() const;

public:
    friend class GraphicsPipelineBuilder;
    friend class ComputePipelineBuilder;

private:
    VkDevice m_device;

    VkPipeline m_pipeline;

    VkPipelineLayout m_pipelineLayout;
};
} // namespace prev::render::pipeline

#endif // !__ABSTRACT_PIPELINE_H__
