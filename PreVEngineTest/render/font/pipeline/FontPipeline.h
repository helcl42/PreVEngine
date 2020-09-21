#ifndef __FONT_PIPELINE_H__
#define __FONT_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::font::pipeline {
class FontPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    FontPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~FontPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::font::pipeline

#endif // !__FONT_PIPELINE_H__
