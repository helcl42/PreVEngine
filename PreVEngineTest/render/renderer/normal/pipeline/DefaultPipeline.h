#ifndef __DEFAULT_PIPELINE_H__
#define __DEFAULT_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::normal::pipeline {
class DefaultPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    DefaultPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const VkRenderPass renderpass, const VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);

    ~DefaultPipeline() = default;

public:
    VkPipeline Init() override;
};

} // namespace prev_test::render::renderer::normal::pipeline

#endif // !__DEFAULT_PIPELINE_H__
