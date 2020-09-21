#ifndef __ANIMATION_PARALLAX_MAPPED_PIPELINE_H__
#define __ANIMATION_PARALLAX_MAPPED_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::animation::pipeline {
class AnimationParallaxMappedPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    AnimationParallaxMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~AnimationParallaxMappedPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::animation::pipeline

#endif