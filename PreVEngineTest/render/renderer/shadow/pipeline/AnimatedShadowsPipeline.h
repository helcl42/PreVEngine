#ifndef __ANIMATED_SHADWS_PIPELINE_H__
#define __ANIMATED_SHADWS_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::shadow::pipeline {
class AnimatedShadowsPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    AnimatedShadowsPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~AnimatedShadowsPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::shadow::pipeline

#endif