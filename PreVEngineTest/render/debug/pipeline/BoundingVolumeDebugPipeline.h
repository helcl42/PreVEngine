#ifndef __BOUNDING_VOLUME_DEBUG_PIPELINE_H__
#define __BOUNDING_VOLUME_DEBUG_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::debug::pipeline {
class BoundingVolumeDebugPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    BoundingVolumeDebugPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~BoundingVolumeDebugPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::debug::pipeline

#endif