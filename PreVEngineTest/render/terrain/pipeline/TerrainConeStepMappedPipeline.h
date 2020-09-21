#ifndef __TERRAIN_CONE_STEP_MAPPED_PIPELINE_H__
#define __TERRAIN_CONE_STEP_MAPPED_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::terrain::pipeline {
class TerrainConeStepMappedPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    TerrainConeStepMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~TerrainConeStepMappedPipeline() = default;

public:
    VkPipeline Init() override;
};

} // namespace prev_test::render::terrain::pipeline

#endif