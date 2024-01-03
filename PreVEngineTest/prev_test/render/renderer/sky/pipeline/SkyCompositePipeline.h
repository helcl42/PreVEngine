#ifndef __SKY_COMPOSITE_PIPELINE_H__
#define __SKY_COMPOSITE_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::sky::pipeline {
class SkyCompositePipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    SkyCompositePipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~SkyCompositePipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::sky::pipeline

#endif // !__SKY_COMPOSITE_PIPELINE_H__
