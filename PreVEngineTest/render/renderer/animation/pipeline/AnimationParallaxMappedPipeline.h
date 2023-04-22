#ifndef __ANIMATION_PARALLAX_MAPPED_PIPELINE_H__
#define __ANIMATION_PARALLAX_MAPPED_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::animation::pipeline {
class AnimationParallaxMappedPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    AnimationParallaxMappedPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~AnimationParallaxMappedPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::animation::pipeline

#endif