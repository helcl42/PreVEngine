#ifndef __SELECTION_DEBUG_PIPELINE_H__
#define __SELECTION_DEBUG_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::debug::pipeline {
class SelectionDebugPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    SelectionDebugPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const VkRenderPass renderpass, const VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);

    ~SelectionDebugPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::debug::pipeline

#endif