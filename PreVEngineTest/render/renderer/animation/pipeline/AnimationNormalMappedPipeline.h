#ifndef __ANIMATION_NORMAL_MAPPED_PIPELINE_H__
#define __ANIMATION_NORMAL_MAPPED_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::animation::pipeline {
class AnimationNormalMappedPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    AnimationNormalMappedPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const VkRenderPass renderpass, const VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);

    ~AnimationNormalMappedPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::animation::pipeline

#endif