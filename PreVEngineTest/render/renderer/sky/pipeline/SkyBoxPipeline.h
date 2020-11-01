#ifndef __SKY_BOX_PIPELINE_H__
#define __SKY_BOX_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::sky::pipeline {
class SkyBoxPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    SkyBoxPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~SkyBoxPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::sky::pipeline

#endif // !__SKY_BOX_PIPELINE_H__
