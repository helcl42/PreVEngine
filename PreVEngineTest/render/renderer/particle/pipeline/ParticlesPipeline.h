#ifndef __PARTICLES_PIPELINE_H__
#define __PARTICLES_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::particle::pipeline {
class ParticlesPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    ParticlesPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~ParticlesPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::particle::pipeline

#endif