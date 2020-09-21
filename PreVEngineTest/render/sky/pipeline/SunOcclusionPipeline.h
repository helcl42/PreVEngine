#ifndef __SUN_OCCLUSION_PIPELINE_H__
#define __SUN_OCCLUSION_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::sky::pipeline {
class SunOcclusionPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    SunOcclusionPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~SunOcclusionPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::sky::pipeline

#endif // !__SUN_OCCLUSION_PIPELINE_H__
