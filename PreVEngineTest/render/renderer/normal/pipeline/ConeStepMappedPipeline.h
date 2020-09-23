#ifndef __CONE_STEP_MAPPED_PIPELINE_H__
#define __CONE_STEP_MAPPED_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::normal::pipeline {
class ConeStepMappedPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    ConeStepMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~ConeStepMappedPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::normal::pipeline

#endif // !__CONE_STEP_MAPPED_PIPELINE_H__
