#ifndef __DEFAULT_SHDOWS_PIPELINE_H__
#define __DEFAULT_SHDOWS_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::shadow::pipeline {
class DefaultShadowsPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    DefaultShadowsPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~DefaultShadowsPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::shadow::pipeline

#endif