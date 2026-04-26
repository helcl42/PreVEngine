#ifndef __GRAPHICS_PIPELINE_BUILDER_H__
#define __GRAPHICS_PIPELINE_BUILDER_H__

#include "AbstractPipelineBuilder.h"

#include "../pass/RenderPass.h"

namespace prev::render::pipeline {
class GraphicsPipelineBuilder final : public AbstractPipelineBuilder {
public:
    GraphicsPipelineBuilder(GfxDevice device, const shader::Shader& shader, const pass::RenderPass& renderPass);

    ~GraphicsPipelineBuilder() = default;

public:
    GraphicsPipelineBuilder& SetPrimitiveTopology(GfxPrimitiveTopology primitiveTopology);

    GraphicsPipelineBuilder& SetDepthTestEnabled(bool enabled);

    GraphicsPipelineBuilder& SetDepthWriteEnabled(bool enabled);

    GraphicsPipelineBuilder& SetBlendingModeEnabled(bool enabled);

    GraphicsPipelineBuilder& SetAdditiveBlendingEnabled(bool enabled);

    GraphicsPipelineBuilder& SetPolygonMode(GfxPolygonMode mode);

    GraphicsPipelineBuilder& SetCullingMode(GfxCullMode mode);

    GraphicsPipelineBuilder& SetFrontFace(GfxFrontFace frontFace);

    std::unique_ptr<Pipeline> Build() const override;

private:
    void Validate() const override;

private:
    GfxRenderPipeline CreateGraphicsPipeline() const;

private:
    const pass::RenderPass& m_renderPass;

    GfxPrimitiveTopology m_primitiveTopology{ GFX_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };

    bool m_depthTestEnabled{ true };

    bool m_depthWriteEnabled{ true };

    bool m_blendingEnabled{ true };

    bool m_additiveBlendingEnabled{ false };

    GfxPolygonMode m_polygonMode{ GFX_POLYGON_MODE_FILL };

    GfxCullMode m_cullingMode{ GFX_CULL_MODE_NONE };

    GfxFrontFace m_frontFace{ GFX_FRONT_FACE_COUNTER_CLOCKWISE };
};
} // namespace prev::render::pipeline

#endif // !__PIPELINE_FACTORY_H__
