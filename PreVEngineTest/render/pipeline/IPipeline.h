#ifndef __IPIPELINE_H__
#define __IPIPELINE_H__

#include <prev/core/instance/Validation.h>

namespace prev_test::render::pipeline {
class IPipeline {
public:
    virtual bool Init() = 0;

    virtual void ShutDown() = 0;

    virtual VkPipelineLayout GetLayout() const = 0;

    virtual operator VkPipeline() const = 0;

public:
    virtual ~IPipeline() = default;
};
} // namespace prev_test::render::pipeline

#endif // !__IPIPELINE_H__
