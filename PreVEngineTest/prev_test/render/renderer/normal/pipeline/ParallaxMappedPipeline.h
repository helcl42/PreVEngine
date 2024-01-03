#ifndef __PARALLAX_MAPPED_PIPELINE_H__
#define __PARALLAX_MAPPED_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::normal::pipeline {
class ParallaxMappedPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    ParallaxMappedPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~ParallaxMappedPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::normal::pipeline

#endif // !__PARALLAX_MAPPED_PIPELINE_H__
