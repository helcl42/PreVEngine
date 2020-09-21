#ifndef __PIPELINE_FACTORY_H__
#define __PIPELINE_FACTORY_H__

#include <render/pass/RenderPass.h>
#include <render/shader/Shader.h>
#include <util/Utils.h>

namespace prev_test::render::pipeline {
class PipelineFactory {
public:
    void CreateShadowsPipeline(const VkDevice& device, const VkRenderPass& renderPass, const prev::render::shader::Shader& shader, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const;

    void CreateDefaultPipeline(const VkDevice& device, const VkRenderPass& renderPass, const prev::render::shader::Shader& shader, const VkPrimitiveTopology topology, const bool depthTestEnabled, const bool depthWriteEnabled, const bool fillMode, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const;

    void CreateParticlesPipeline(const VkDevice& device, const VkRenderPass& renderPass, const prev::render::shader::Shader& shader, const VkPrimitiveTopology topology, const bool depthTestEnabled, const bool depthWriteEnabled, const bool fillMode, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const;

    void CreateDebugPipeline(const VkDevice& device, const VkRenderPass& renderPass, const prev::render::shader::Shader& shader, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const;

    void CreateDefaultComputePipeline(const VkDevice& device, const prev::render::shader::Shader& shader, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const;
};
} // namespace prev_test::render::pipeline

#endif // !__PIPELINE_FACTORY_H__
