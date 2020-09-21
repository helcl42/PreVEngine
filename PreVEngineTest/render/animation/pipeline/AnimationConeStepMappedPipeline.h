#ifndef __ANIMATION_CONE_STEP_MAPPED_PIPELINE_H__
#define __ANIMATION_CONE_STEP_MAPPED_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::animation::pipeline {
class AnimationConeStepMappedPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    AnimationConeStepMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~AnimationConeStepMappedPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::animation::pipeline

#endif