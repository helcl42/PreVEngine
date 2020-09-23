#ifndef __ANIMATED_SHADWS_PIPELINE_H__
#define __ANIMATED_SHADWS_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::shadow::pipeline {
class AnimatedShadowsPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    AnimatedShadowsPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~AnimatedShadowsPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::shadow::pipeline

#endif