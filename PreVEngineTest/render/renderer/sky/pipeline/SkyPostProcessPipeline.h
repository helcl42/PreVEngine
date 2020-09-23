#ifndef __SKY_POST_PROCESS_PIPELINE_H__
#define __SKY_POST_PROCESS_PIPELINE_H__

#include "../../../pipeline/AbstractComputePipeline.h"

namespace prev_test::render::renderer::sky::pipeline {
class SkyPostProcessPipeline final : public prev_test::render::pipeline::AbstractComputePipeline {
public:
    SkyPostProcessPipeline(const VkDevice device, const prev::render::shader::Shader& shaders);

    ~SkyPostProcessPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::sky::pipeline

#endif // !__SKY_POST_PROCESS_PIPELINE_H__
