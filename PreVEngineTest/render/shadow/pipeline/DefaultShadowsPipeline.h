#ifndef __DEFAULT_SHDOWS_PIPELINE_H__
#define __DEFAULT_SHDOWS_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::shadow::pipeline {
class DefaultShadowsPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    DefaultShadowsPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~DefaultShadowsPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::shadow::pipeline

#endif