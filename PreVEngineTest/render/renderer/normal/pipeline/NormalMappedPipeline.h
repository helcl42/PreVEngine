#ifndef __NORMAL_MAPPED_PIPELINE_H__
#define __NORMAL_MAPPED_PIPELINE_H__

#include "../../../pipeline/AbstractGraphicsPipeline.h"

namespace prev_test::render::renderer::normal::pipeline {
class NormalMappedPipeline final : public prev_test::render::pipeline::AbstractGraphicsPipeline {
public:
    NormalMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const prev::render::shader::Shader& shaders);

    ~NormalMappedPipeline() = default;

public:
    VkPipeline Init() override;
};
} // namespace prev_test::render::renderer::normal::pipeline

#endif // !__NORMAL_MAPPED_PIPELINE_H__
