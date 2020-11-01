#ifndef __SUN_OCCLUSION_PIPELINE_H__
#define __SUN_OCCLUSION_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::sky::pipeline {
class SunOcclusionPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    SunOcclusionPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const VkRenderPass renderpass, const VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);

    ~SunOcclusionPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::sky::pipeline

#endif // !__SUN_OCCLUSION_PIPELINE_H__
