#ifndef __FONT_PIPELINE_H__
#define __FONT_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::font::pipeline {
class FontPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    FontPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const VkRenderPass renderpass, const VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);

    ~FontPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::font::pipeline

#endif // !__FONT_PIPELINE_H__
