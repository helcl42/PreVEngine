#ifndef __PIPELINE_FACTORY_H__
#define __PIPELINE_FACTORY_H__

#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/util/Utils.h>

namespace prev_test::render::pipeline {
class PipelineFactory {
public:
    void CreateShadowsPipeline(const VkDevice& device, const prev::render::shader::Shader& shader, const VkRenderPass& renderPass, const VkSampleCountFlagBits sampleCount, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const;

    void CreateDefaultPipeline(const VkDevice& device, const prev::render::shader::Shader& shader, const VkRenderPass& renderPass, const VkSampleCountFlagBits sampleCount, const VkPrimitiveTopology topology, const bool depthTestEnabled, const bool depthWriteEnabled, const bool fillMode, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const;

    void CreateParticlesPipeline(const VkDevice& device, const prev::render::shader::Shader& shader, const VkRenderPass& renderPass, const VkSampleCountFlagBits sampleCount, const VkPrimitiveTopology topology, const bool depthTestEnabled, const bool depthWriteEnabled, const bool fillMode, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const;

    void CreateDebugPipeline(const VkDevice& device, const prev::render::shader::Shader& shader, const VkRenderPass& renderPass, const VkSampleCountFlagBits sampleCount, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const;

    void CreateDefaultComputePipeline(const VkDevice& device, const prev::render::shader::Shader& shader, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const;
};
} // namespace prev_test::render::pipeline

#endif // !__PIPELINE_FACTORY_H__
