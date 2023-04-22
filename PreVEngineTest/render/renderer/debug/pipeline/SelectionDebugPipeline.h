#ifndef __SELECTION_DEBUG_PIPELINE_H__
#define __SELECTION_DEBUG_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::debug::pipeline {
class SelectionDebugPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    SelectionDebugPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~SelectionDebugPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::debug::pipeline

#endif