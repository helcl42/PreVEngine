#ifndef __RAY_CAST_DEBUG_PIPELINE_H__
#define __RAY_CAST_DEBUG_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::debug::pipeline {
class RayCastDebugPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    RayCastDebugPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~RayCastDebugPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::debug::pipeline

#endif // !__RAY_CAST_DEBUG_SHADER_H__
