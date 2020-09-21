#ifndef __SHADOW_MAP_DEBUG_PIPELINE_H__
#define __SHADOW_MAP_DEBUG_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::debug::pipeline {
class ShadowMapDebugPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    ShadowMapDebugPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~ShadowMapDebugPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::debug::pipeline

#endif