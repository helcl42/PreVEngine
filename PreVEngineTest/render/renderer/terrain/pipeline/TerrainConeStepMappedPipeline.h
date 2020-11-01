#ifndef __TERRAIN_CONE_STEP_MAPPED_PIPELINE_H__
#define __TERRAIN_CONE_STEP_MAPPED_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::terrain::pipeline {
class TerrainConeStepMappedPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    TerrainConeStepMappedPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const VkRenderPass renderpass, const VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);

    ~TerrainConeStepMappedPipeline() = default;

public:
    VkPipeline Init() override;
};

} // namespace prev_test::render::renderer::terrain::pipeline

#endif