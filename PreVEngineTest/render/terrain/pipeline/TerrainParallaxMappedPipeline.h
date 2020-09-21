#ifndef __TERRAIN_PARALLAX_MAPPED_PIPELINE_H__
#define __TERRAIN_PARALLAX_MAPPED_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::terrain::pipeline {
class TerrainParallaxMappedPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    TerrainParallaxMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~TerrainParallaxMappedPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::terrain::pipeline

#endif