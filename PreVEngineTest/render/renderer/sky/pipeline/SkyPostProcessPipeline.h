#ifndef __SKY_POST_PROCESS_PIPELINE_H__
#define __SKY_POST_PROCESS_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::sky::pipeline {
class SkyPostProcessPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    SkyPostProcessPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~SkyPostProcessPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::sky::pipeline

#endif // !__SKY_POST_PROCESS_PIPELINE_H__
