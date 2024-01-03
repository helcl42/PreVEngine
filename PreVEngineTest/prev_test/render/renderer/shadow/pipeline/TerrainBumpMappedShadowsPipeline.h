#ifndef __TERRAIN_BUMP_MAPPED_SHDOWS_PIPELINE_H__
#define __TERRAIN_BUMP_MAPPED_SHDOWS_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::shadow::pipeline {
class TerrainBumpMappedShadowsPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    TerrainBumpMappedShadowsPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~TerrainBumpMappedShadowsPipeline() = default;

public:
    bool Init() override;
};

} // namespace prev_test::render::renderer::shadow::pipeline

#endif