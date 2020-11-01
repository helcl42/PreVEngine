#ifndef __SHADOW_MAP_DEBUG_PIPELINE_H__
#define __SHADOW_MAP_DEBUG_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::debug::pipeline {
class ShadowMapDebugPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    ShadowMapDebugPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~ShadowMapDebugPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::debug::pipeline

#endif