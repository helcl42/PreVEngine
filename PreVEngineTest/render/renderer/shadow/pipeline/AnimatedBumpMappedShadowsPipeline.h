#ifndef __ANIMATED_BUMP_MAPPED_SHADOWS_PIPELINE_H__
#define __ANIMATED_BUMP_MAPPED_SHADOWS_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::shadow::pipeline {
class AnimatedBumpMappedShadowsPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    AnimatedBumpMappedShadowsPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~AnimatedBumpMappedShadowsPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::shadow::pipeline

#endif