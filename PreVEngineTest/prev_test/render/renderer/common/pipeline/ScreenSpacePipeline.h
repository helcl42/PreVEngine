#ifndef __SCREE_SPACE_PIPELINE_H__
#define __SCREE_SPACE_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::common::pipeline {
class ScreenSpacePipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    ScreenSpacePipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~ScreenSpacePipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::common::pipeline

#endif // !__SCREE_SPACE_PIPELINE_H__
