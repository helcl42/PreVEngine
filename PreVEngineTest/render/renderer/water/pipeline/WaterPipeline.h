#ifndef __WATER_PIPELINE_H__
#define __WATER_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::water::pipeline {
class WaterPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    WaterPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~WaterPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::water::pipeline

#endif