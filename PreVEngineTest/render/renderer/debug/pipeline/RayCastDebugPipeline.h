#ifndef __RAY_CAST_DEBUG_PIPELINE_H__
#define __RAY_CAST_DEBUG_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::debug::pipeline {
class RayCastDebugPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    RayCastDebugPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~RayCastDebugPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::debug::pipeline

#endif // !__RAY_CAST_DEBUG_SHADER_H__
