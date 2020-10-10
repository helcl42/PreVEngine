#ifndef __CLOUDS_PERLIN_WORLEY_NOISE_PIPELINE_H__
#define __CLOUDS_PERLIN_WORLEY_NOISE_PIPELINE_H__

#include "../../../pipeline/AbstractComputePipeline.h"

namespace prev_test::render::renderer::sky::pipeline {
class CloudsPerlinWorleyNoisePipeline final : public prev_test::render::pipeline::AbstractComputePipeline {
public:
    CloudsPerlinWorleyNoisePipeline(const VkDevice device, const prev::render::shader::Shader& shaders);

    ~CloudsPerlinWorleyNoisePipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::sky::pipeline

#endif // !__PERLIN_WORLEY_NOISE_PIPELINE_H__
