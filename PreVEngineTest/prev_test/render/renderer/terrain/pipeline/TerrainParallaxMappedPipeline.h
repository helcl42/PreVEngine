#ifndef __TERRAIN_PARALLAX_MAPPED_PIPELINE_H__
#define __TERRAIN_PARALLAX_MAPPED_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::terrain::pipeline {
class TerrainParallaxMappedPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    TerrainParallaxMappedPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~TerrainParallaxMappedPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::terrain::pipeline

#endif