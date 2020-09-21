#ifndef __TEXTURE_DEBUG_PIPELINE_H__
#define __TEXTURE_DEBUG_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::debug::pipeline {
class TextureDebugPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    TextureDebugPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~TextureDebugPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::debug::pipeline

#endif