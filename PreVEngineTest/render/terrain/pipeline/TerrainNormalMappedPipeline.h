#ifndef __TERRAIN_NORMAL_MAPPED_PIPELINE_H__
#define __TERRAIN_NORMAL_MAPPED_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::terrain::pipeline {
class TerrainNormalMappedPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    TerrainNormalMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~TerrainNormalMappedPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::terrain::pipeline

#endif