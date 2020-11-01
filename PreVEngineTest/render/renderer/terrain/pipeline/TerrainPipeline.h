#ifndef __TERRAIN_PIPELINE_H__
#define __TERRAIN_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::terrain::pipeline {
class TerrainPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    TerrainPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~TerrainPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::terrain::pipeline

#endif