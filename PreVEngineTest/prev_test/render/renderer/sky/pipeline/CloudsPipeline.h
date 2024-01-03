#ifndef __CLOUDS_PIPELINE_H__
#define __CLOUDS_PIPELINE_H__

#include "../../../pipeline/AbstractComputePipeline.h"

namespace prev_test::render::renderer::sky::pipeline {
class CloudsPipeline final : public prev_test::render::pipeline::AbstractComputePipeline {
public:
    CloudsPipeline(const VkDevice device, const prev::render::shader::Shader& shaders);

    ~CloudsPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::sky::pipeline

#endif // !__CLOUDS_PIPELINE_H__
