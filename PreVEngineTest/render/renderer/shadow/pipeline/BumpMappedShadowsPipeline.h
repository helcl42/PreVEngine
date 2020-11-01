#ifndef __BUMP_MAPPED_SHADOWS_PIPELINE_H__
#define __BUMP_MAPPED_SHADOWS_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::shadow::pipeline {
class BumpMappedShadowsPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    BumpMappedShadowsPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~BumpMappedShadowsPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::shadow::pipeline

#endif // !__BUMP_MAPPED_SHADOWS_PIPELINE_H__
