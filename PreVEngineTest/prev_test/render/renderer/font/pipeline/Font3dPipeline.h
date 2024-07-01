#ifndef __FONT_3D_PIPELINE_H__
#define __FONT_3D_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::font::pipeline {
class Font3dPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    Font3dPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~Font3dPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::font::pipeline

#endif // !__FONT_3D_PIPELINE_H__
