#ifndef __FONT_PIPELINE_H__
#define __FONT_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::font::pipeline {
class FontPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    FontPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~FontPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::font::pipeline

#endif // !__FONT_PIPELINE_H__
