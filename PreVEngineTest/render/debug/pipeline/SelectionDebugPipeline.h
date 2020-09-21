#ifndef __SELECTION_DEBUG_PIPELINE_H__
#define __SELECTION_DEBUG_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::debug::pipeline {
class SelectionDebugPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    SelectionDebugPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~SelectionDebugPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::debug::pipeline

#endif