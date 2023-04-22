#ifndef __TEXTURE_DEBUG_PIPELINE_H__
#define __TEXTURE_DEBUG_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::debug::pipeline {
class TextureDebugPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    TextureDebugPipeline(const VkDevice device, const prev::render::shader::Shader& shaders, const prev::render::pass::RenderPass& renderPass);

    ~TextureDebugPipeline() = default;

public:
    bool Init() override;
};
} // namespace prev_test::render::renderer::debug::pipeline

#endif