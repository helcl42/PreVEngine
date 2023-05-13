#ifndef __PIPELINE_FACTORY_H__
#define __PIPELINE_FACTORY_H__

#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>

#include <prev/util/Utils.h>

namespace prev_test::render::pipeline {
class PipelineFactory {
public:
    VkPipelineLayout CreatePipelineLayout(const VkDevice& device, const prev::render::shader::Shader& shader);

    VkPipeline CreateShadowsPipeline(const VkDevice& device, const prev::render::shader::Shader& shader, const prev::render::pass::RenderPass& renderPass, const VkPipelineLayout pipelineLayout) const;

    VkPipeline CreateDefaultPipeline(const VkDevice& device, const prev::render::shader::Shader& shader, const prev::render::pass::RenderPass& renderPass, const VkPipelineLayout pipelineLayout, const VkPrimitiveTopology topology, const bool depthTestEnabled, const bool depthWriteEnabled, const bool blendingEnabled, const bool fillMode) const;

    VkPipeline CreateParticlesPipeline(const VkDevice& device, const prev::render::shader::Shader& shader, const prev::render::pass::RenderPass& renderPass, const VkPipelineLayout pipelineLayout, const VkPrimitiveTopology topology, const bool depthTestEnabled, const bool depthWriteEnabled, const bool fillMode) const;

    VkPipeline CreateDefaultComputePipeline(const VkDevice& device, const prev::render::shader::Shader& shader, const VkPipelineLayout pipelineLayout) const;
};
} // namespace prev_test::render::pipeline

#endif // !__PIPELINE_FACTORY_H__
