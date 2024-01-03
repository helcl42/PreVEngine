#ifndef __FLARE_PIPELINE_H__
#define __FLARE_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::sky::pipeline {
class FlarePipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    FlarePipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~FlarePipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::sky::pipeline

#endif // !__FLARE_PIPELINE_H__
