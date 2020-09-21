#ifndef __DEFAULT_PIPELINE_H__
#define __DEFAULT_PIPELINE_H__

#include "../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::normal::pipeline {
class DefaultPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    DefaultPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~DefaultPipeline() = default;

public:
    VkPipeline Init() override;
};

} // namespace prev_test::render::normal::pipeline

#endif // !__DEFAULT_PIPELINE_H__
