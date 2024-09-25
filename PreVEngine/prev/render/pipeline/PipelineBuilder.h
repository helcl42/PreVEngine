#ifndef __PIPELINE_FACTORY_H__
#define __PIPELINE_FACTORY_H__

#include "../pass/RenderPass.h"
#include "../shader/Shader.h"

#include "Pipeline.h"

namespace prev::render::pipeline {
class ComputePipelineBuilder final {
public:
    ComputePipelineBuilder(const VkDevice device, const shader::Shader& shader);

    ~ComputePipelineBuilder() = default;

public:
    std::unique_ptr<Pipeline> Build() const;

private:
    const VkDevice m_device;

    const shader::Shader& m_shader;
};

class GraphicsPipelineBuilder final {
public:
    GraphicsPipelineBuilder(const VkDevice device, const shader::Shader& shader, const pass::RenderPass& renderPass);

    ~GraphicsPipelineBuilder() = default;

public:
    GraphicsPipelineBuilder& SetPrimitiveTopology(VkPrimitiveTopology primitiveTopology);

    GraphicsPipelineBuilder& SetDepthTestEnabled(bool enabled);

    GraphicsPipelineBuilder& SetDepthWriteEnabled(bool enabled);

    GraphicsPipelineBuilder& SetBlendingModeEnabled(bool enabled);

    GraphicsPipelineBuilder& SetAdditiveBlendingEnabled(bool enabled);

    GraphicsPipelineBuilder& SetPatchControlPointCount(uint32_t count);

    GraphicsPipelineBuilder& SetPolygonMode(VkPolygonMode mode);

    GraphicsPipelineBuilder& SetCullingMode(VkCullModeFlagBits mode);

    GraphicsPipelineBuilder& SetFrontFace(VkFrontFace frontFace);

    GraphicsPipelineBuilder& SetSampleShadingEnabled(bool enabled);

    GraphicsPipelineBuilder& SetSampleShadingMinimumFraction(float fraction);

    std::unique_ptr<Pipeline> Build() const;

private:
    const VkDevice m_device;

    const shader::Shader& m_shader;

    const pass::RenderPass& m_renderPass;

    VkPrimitiveTopology m_primitiveTopology{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };

    bool m_depthTestEnabled{ true };

    bool m_depthWriteEnabled{ true };

    bool m_blendingEnabled{ true };

    bool m_additiveBlendingEnabled{ false };

    uint32_t m_patchControlPointCount{ 0 };

    VkPolygonMode m_polygonMode{ VK_POLYGON_MODE_FILL };

    VkCullModeFlagBits m_cullingMode{ VK_CULL_MODE_NONE };

    VkFrontFace m_frontFace{ VK_FRONT_FACE_COUNTER_CLOCKWISE };

    bool m_sampleShadingEnabled{ false };

    float m_sampleShadingMinFraction{ 0.2f };
};
} // namespace prev::render::pipeline

#endif // !__PIPELINE_FACTORY_H__
