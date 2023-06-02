#ifndef __SKY_PIPELINE_H__
#define __SKY_PIPELINE_H__

#include "../../../pipeline/AbstractComputePipeline.h"

namespace prev_test::render::renderer::sky::pipeline {
class SkyPipeline final : public prev_test::render::pipeline::AbstractComputePipeline {
public:
    SkyPipeline(const VkDevice device, const prev::render::shader::Shader& shaders);

    ~SkyPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::sky::pipeline

#endif // !__SKY_PIPELINE_H__
