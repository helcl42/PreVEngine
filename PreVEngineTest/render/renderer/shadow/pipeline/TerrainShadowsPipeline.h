#ifndef __TERRAIN_SHADOWS_PIPELINE_H__
#define __TERRAIN_SHADOWS_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::shadow::pipeline {
class TerrainShadowsPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    TerrainShadowsPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~TerrainShadowsPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::shadow::pipeline

#endif