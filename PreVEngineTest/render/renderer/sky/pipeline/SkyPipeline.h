#ifndef __SKY_PIPELINE_H__
#define __SKY_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::sky::pipeline {
class SkyPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    SkyPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~SkyPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::sky::pipeline

#endif // !__SKY_PIPELINE_H__
