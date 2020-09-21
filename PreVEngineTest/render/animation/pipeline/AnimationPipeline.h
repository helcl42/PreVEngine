#ifndef __ANIMATION_PIPELINE_H__
#define __ANIMATION_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::animation::pipeline {
class AnimationPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    AnimationPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~AnimationPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::animation::pipeline

#endif // !__ANIMATION_PIPELINE_H__
