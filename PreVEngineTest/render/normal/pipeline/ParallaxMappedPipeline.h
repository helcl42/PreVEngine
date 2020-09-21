#ifndef __PARALLAX_MAPPED_PIPELINE_H__
#define __PARALLAX_MAPPED_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::normal::pipeline {
class ParallaxMappedPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    ParallaxMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~ParallaxMappedPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::normal::pipeline

#endif // !__PARALLAX_MAPPED_PIPELINE_H__
