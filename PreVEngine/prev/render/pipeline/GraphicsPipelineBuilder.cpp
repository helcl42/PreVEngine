#include "GraphicsPipelineBuilder.h"

#include "../../common/Logger.h"

#include <prev/core/Formats.h>

#include <algorithm>
#include <map>
#include <stdexcept>
#include <vector>

namespace prev::render::pipeline {
GraphicsPipelineBuilder::GraphicsPipelineBuilder(GfxDevice device, const shader::Shader& shader, const pass::RenderPass& renderPass)
    : AbstractPipelineBuilder(device, shader)
    , m_renderPass{ renderPass }
{
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetPrimitiveTopology(GfxPrimitiveTopology primitiveTopology)
{
    m_primitiveTopology = primitiveTopology;
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetDepthTestEnabled(bool enabled)
{
    m_depthTestEnabled = enabled;
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetDepthWriteEnabled(bool enabled)
{
    m_depthWriteEnabled = enabled;
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetBlendingModeEnabled(bool enabled)
{
    m_blendingEnabled = enabled;
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetAdditiveBlendingEnabled(bool enabled)
{
    m_additiveBlendingEnabled = enabled;
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetPolygonMode(GfxPolygonMode mode)
{
    m_polygonMode = mode;
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetCullingMode(GfxCullMode mode)
{
    m_cullingMode = mode;
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetFrontFace(GfxFrontFace frontFace)
{
    m_frontFace = frontFace;
    return *this;
}

std::unique_ptr<Pipeline> GraphicsPipelineBuilder::Build() const
{
    Validate();

    auto pipeline{ CreateGraphicsPipeline() };

    return std::unique_ptr<Pipeline>(new Pipeline(m_device, pipeline));
}

GfxRenderPipeline GraphicsPipelineBuilder::CreateGraphicsPipeline() const
{
    // --- Vertex state: group attributes by binding ---
    const auto& inputBindings{ m_shader.GetVertexInputBindings() };
    const auto& inputAttributes{ m_shader.GetVertexInputAttributes() };

    // Build per-binding GfxVertexAttribute arrays
    std::map<uint32_t, std::vector<GfxVertexAttribute>> attrsByBinding;
    for (const auto& attr : inputAttributes) {
        GfxVertexAttribute gfxAttr{};
        gfxAttr.format = attr.format;
        gfxAttr.offset = static_cast<uint64_t>(attr.offset);
        gfxAttr.shaderLocation = attr.shaderLocation;
        attrsByBinding[attr.binding].push_back(gfxAttr);
    }

    std::vector<GfxVertexBufferLayout> bufferLayouts;
    bufferLayouts.reserve(inputBindings.size());
    for (const auto& binding : inputBindings) {
        GfxVertexBufferLayout layout{};
        layout.arrayStride = static_cast<uint64_t>(binding.stride);
        layout.stepMode = binding.stepMode;
        const auto it{ attrsByBinding.find(binding.binding) };
        if (it != attrsByBinding.end()) {
            layout.attributes = it->second.data();
            layout.attributeCount = static_cast<uint32_t>(it->second.size());
        } else {
            layout.attributes = nullptr;
            layout.attributeCount = 0;
        }
        bufferLayouts.push_back(layout);
    }

    // Find vertex and fragment shader modules
    GfxShader vertexShader{};
    GfxShader fragmentShader{};
    for (const auto& [stage, shader] : m_shader.GetShaderModules()) {
        if (stage == GFX_SHADER_STAGE_VERTEX) {
            vertexShader = shader;
        } else if (stage == GFX_SHADER_STAGE_FRAGMENT) {
            fragmentShader = shader;
        }
    }

    GfxVertexState vertexState{};
    vertexState.module = vertexShader;
    vertexState.entryPoint = "vertexMain";
    vertexState.buffers = bufferLayouts.data();
    vertexState.bufferCount = static_cast<uint32_t>(bufferLayouts.size());

    // --- Fragment state: color targets ---
    const auto gfxColorFormats{ m_renderPass.GetGfxColorFormats() };

    GfxBlendState blendState{};
    if (m_additiveBlendingEnabled) {
        blendState.color = { GFX_BLEND_OPERATION_ADD, GFX_BLEND_FACTOR_ONE, GFX_BLEND_FACTOR_ONE };
        blendState.alpha = { GFX_BLEND_OPERATION_ADD, GFX_BLEND_FACTOR_SRC_ALPHA, GFX_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA };
    } else {
        blendState.color = { GFX_BLEND_OPERATION_ADD, GFX_BLEND_FACTOR_SRC_ALPHA, GFX_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA };
        blendState.alpha = { GFX_BLEND_OPERATION_ADD, GFX_BLEND_FACTOR_ONE, GFX_BLEND_FACTOR_ZERO };
    }

    std::vector<GfxColorTargetState> colorTargets;
    colorTargets.reserve(gfxColorFormats.size());
    for (const auto& fmt : gfxColorFormats) {
        GfxColorTargetState target{};
        target.format = fmt;
        target.blend = m_blendingEnabled ? &blendState : nullptr;
        target.writeMask = GFX_COLOR_WRITE_MASK_ALL;
        colorTargets.push_back(target);
    }

    GfxFragmentState fragmentState{};
    fragmentState.module = fragmentShader;
    fragmentState.entryPoint = "fragmentMain";
    fragmentState.targets = colorTargets.data();
    fragmentState.targetCount = static_cast<uint32_t>(colorTargets.size());

    // --- Primitive state ---
    GfxPrimitiveState primitiveState{};
    primitiveState.topology = m_primitiveTopology;
    primitiveState.stripIndexFormat = GFX_INDEX_FORMAT_UNDEFINED;
    primitiveState.frontFace = m_frontFace;
    primitiveState.cullMode = m_cullingMode;
    primitiveState.polygonMode = m_polygonMode;

    // --- Depth/stencil state ---
    GfxDepthStencilState depthStencilState{};
    depthStencilState.format = m_renderPass.GetDepthFormat();
    depthStencilState.depthWriteEnabled = m_depthWriteEnabled;
    if (m_depthTestEnabled) {
        if constexpr (REVERSE_DEPTH) {
            depthStencilState.depthCompare = GFX_COMPARE_FUNCTION_GREATER_EQUAL;
        } else {
            depthStencilState.depthCompare = GFX_COMPARE_FUNCTION_LESS_EQUAL;
        }
    } else {
        depthStencilState.depthCompare = GFX_COMPARE_FUNCTION_ALWAYS;
    }

    // --- Bind group layout ---
    GfxBindGroupLayout bindGroupLayout{ m_shader.GetBindGroupLayout() };

    // --- Assemble pipeline descriptor ---
    GfxRenderPipelineDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_RENDER_PIPELINE_DESCRIPTOR;
    desc.renderPass = m_renderPass;
    desc.vertex = &vertexState;
    desc.fragment = fragmentShader ? &fragmentState : nullptr;
    desc.primitive = &primitiveState;
    desc.depthStencil = (depthStencilState.format != GFX_FORMAT_UNDEFINED) ? &depthStencilState : nullptr;
    desc.sampleCount = m_renderPass.GetSampleCount();
    desc.bindGroupLayouts = &bindGroupLayout;
    desc.bindGroupLayoutCount = 1;

    GfxRenderPipeline pipeline{};
    GFXERRCHECK(gfxDeviceCreateRenderPipeline(m_device, &desc, &pipeline));
    return pipeline;
}

void GraphicsPipelineBuilder::Validate() const
{
    if (!m_depthTestEnabled && m_depthWriteEnabled) {
        throw std::runtime_error("Invalid pipeline configuration: Depth test is disabled but depth write enabled - depth write value will be ignored.");
    }

    if (!m_blendingEnabled && m_additiveBlendingEnabled) {
        throw std::runtime_error("Invalid pipeline configuration: Blending is disabled and additive blending enabled - additive blending will be ignored.");
    }
}
} // namespace prev::render::pipeline
