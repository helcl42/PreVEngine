#ifndef __ANIMATED_BUMP_MAPPED_SHADOWS_PIPELINE_H__
#define __ANIMATED_BUMP_MAPPED_SHADOWS_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::shadow::pipeline {
class AnimatedBumpMappedShadowsPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    AnimatedBumpMappedShadowsPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const VkRenderPass renderpass, const VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);

    ~AnimatedBumpMappedShadowsPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::shadow::pipeline

#endif