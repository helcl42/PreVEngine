#ifndef __FLARE_PIPELINE_H__
#define __FLARE_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::sky::pipeline {
class FlarePipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    FlarePipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~FlarePipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::sky::pipeline

#endif // !__FLARE_PIPELINE_H__
