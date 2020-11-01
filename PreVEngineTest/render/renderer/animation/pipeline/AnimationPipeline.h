#ifndef __ANIMATION_PIPELINE_H__
#define __ANIMATION_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::animation::pipeline {
class AnimationPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    AnimationPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const VkRenderPass renderpass, const VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);

    ~AnimationPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::animation::pipeline

#endif // !__ANIMATION_PIPELINE_H__
