#ifndef __ANIMATION_TEXTURELESS_PIPELINE_H__
#define __ANIMATION_TEXTURELESS_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::animation::pipeline {
class AnimationTexturelessPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    AnimationTexturelessPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~AnimationTexturelessPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::animation::pipeline

#endif // !__ANIMATION_TEXTURELESS_PIPELINE_H__
