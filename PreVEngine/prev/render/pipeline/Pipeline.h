#ifndef __PIPELINE_H__
#define __PIPELINE_H__
#include "../../core/Core.h"

namespace prev::render::pipeline {
class Pipeline final {
public:
    Pipeline(const VkDevice device, const VkPipeline pipeline, const VkPipelineLayout pipelineLayout);

    ~Pipeline();

public:
    VkPipelineLayout GetLayout() const;

    operator VkPipeline() const;

private:
    VkDevice m_device;

    VkPipeline m_pipeline;

    VkPipelineLayout m_pipelineLayout;
};
} // namespace prev::render::pipeline

#endif // !__ABSTRACT_PIPELINE_H__
