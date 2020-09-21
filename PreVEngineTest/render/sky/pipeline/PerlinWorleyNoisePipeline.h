#ifndef __PERLIN_WORLEY_NOISE_PIPELINE_H__
#define __PERLIN_WORLEY_NOISE_PIPELINE_H__

#include "../../pipeline/AbstractComputePipeline.h"

namespace prev_test::render::sky::pipeline {
class PerlinWorleyNoisePipeline final : public prev_test::render::pipeline::AbstractComputePipeline {
public:
    PerlinWorleyNoisePipeline(const VkDevice device, const prev::render::shader::Shader& shaders);

    ~PerlinWorleyNoisePipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::sky::pipeline

#endif // !__PERLIN_WORLEY_NOISE_PIPELINE_H__
