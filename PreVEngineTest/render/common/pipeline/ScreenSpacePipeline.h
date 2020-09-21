#ifndef __SCREE_SPACE_PIPELINE_H__
#define __SCREE_SPACE_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::common::pipeline {
class ScreenSpacePipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    ScreenSpacePipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~ScreenSpacePipeline() = default;

public:
    VkPipeline Init() override;
};
}

#endif // !__SCREE_SPACE_PIPELINE_H__
