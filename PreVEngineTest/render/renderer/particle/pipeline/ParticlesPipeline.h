#ifndef __PARTICLES_PIPELINE_H__
#define __PARTICLES_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::particle::pipeline {
class ParticlesPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    ParticlesPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~ParticlesPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::particle::pipeline

#endif