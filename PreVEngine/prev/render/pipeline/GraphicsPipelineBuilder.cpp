#include "GraphicsPipelineBuilder.h"

#include "../../util/Utils.h"
#include "../../util/VkUtils.h"

#include <stdexcept>
#include <vector>

namespace prev::render::pipeline {
namespace {
    VkPipelineInputAssemblyStateCreateInfo CreatePipelineInputAssemblyStateCreateInfo(const VkPrimitiveTopology topology)
    {
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{ prev::util::vk::CreateStruct<VkPipelineInputAssemblyStateCreateInfo>(VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO) };
        inputAssembly.topology = topology;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        return inputAssembly;
    }

    VkPipelineVertexInputStateCreateInfo CreatePipelineVertexInputStateCreateInfo(const std::vector<VkVertexInputBindingDescription>& vertexInputBindingDescription, const std::vector<VkVertexInputAttributeDescription>& vertexInputAttributeDescription)
    {
        VkPipelineVertexInputStateCreateInfo vertexInput{ prev::util::vk::CreateStruct<VkPipelineVertexInputStateCreateInfo>(VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO) };
        vertexInput.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindingDescription.size());
        vertexInput.pVertexBindingDescriptions = vertexInputBindingDescription.data();
        vertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescription.size());
        vertexInput.pVertexAttributeDescriptions = vertexInputAttributeDescription.data();
        return vertexInput;
    }

    VkPipelineViewportStateCreateInfo CreatePipelineViewportStateCreateInfo(const VkViewport& viewport, const VkRect2D& scissor)
    {
        VkPipelineViewportStateCreateInfo viewportStateCreateInfo{ prev::util::vk::CreateStruct<VkPipelineViewportStateCreateInfo>(VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO) };
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.pViewports = &viewport;
        viewportStateCreateInfo.scissorCount = 1;
        viewportStateCreateInfo.pScissors = &scissor;
        return viewportStateCreateInfo;
    }

    VkPipelineRasterizationStateCreateInfo CreatePipelineRasterizationStateCreateInfo(const VkPolygonMode polygonMode, const VkCullModeFlagBits cullingMode, const VkFrontFace frontFace)
    {
        VkPipelineRasterizationStateCreateInfo rasterizer{ prev::util::vk::CreateStruct<VkPipelineRasterizationStateCreateInfo>(VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO) };
        rasterizer.depthClampEnable = VK_TRUE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = polygonMode;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = cullingMode;
        rasterizer.frontFace = frontFace;
        rasterizer.depthBiasEnable = VK_FALSE;
        return rasterizer;
    }

    VkPipelineMultisampleStateCreateInfo CreatePipelineMultisampleStateCreateInfo(const VkSampleCountFlagBits sampleCount, const bool sampleShadingEnabled, const float sampleShadingFraction)
    {
        VkPipelineMultisampleStateCreateInfo multisampling{ prev::util::vk::CreateStruct<VkPipelineMultisampleStateCreateInfo>(VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO) };
        if (sampleCount > VK_SAMPLE_COUNT_1_BIT) {
            multisampling.sampleShadingEnable = sampleShadingEnabled;
            multisampling.rasterizationSamples = sampleCount;
            multisampling.minSampleShading = sampleShadingFraction;
        } else {
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = sampleCount;
        }
        return multisampling;
    }

    VkPipelineDepthStencilStateCreateInfo CreatePipelineDepthStencilStateCreateInfo(const bool depthTestEnabled, const bool depthWriteEnabled)
    {
        VkPipelineDepthStencilStateCreateInfo depthStencil{ prev::util::vk::CreateStruct<VkPipelineDepthStencilStateCreateInfo>(VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO) };
        depthStencil.depthTestEnable = depthTestEnabled ? VK_TRUE : VK_FALSE;
        depthStencil.depthWriteEnable = depthWriteEnabled ? VK_TRUE : VK_FALSE;
        if constexpr (REVERSE_DEPTH) {
            depthStencil.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
        } else {
            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        }
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;
        return depthStencil;
    }

    std::vector<VkPipelineColorBlendAttachmentState> CreatePipelineColorBlendAttachmentStates(const bool blendingEnabled, const bool additiveBlendingEnabled, const uint32_t colorBlenAttchmentCount)
    {
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(colorBlenAttchmentCount);
        for (uint32_t i = 0; i < colorBlenAttchmentCount; ++i) {
            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = blendingEnabled ? VK_TRUE : VK_FALSE;
            if (additiveBlendingEnabled) {
                colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
                colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
            } else {
                colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
                colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
            }
            colorBlendAttachments[i] = colorBlendAttachment;
        }
        return colorBlendAttachments;
    }

    VkPipelineColorBlendStateCreateInfo CreatePipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& colorBlendAttachments)
    {
        VkPipelineColorBlendStateCreateInfo colorBlending{ prev::util::vk::CreateStruct<VkPipelineColorBlendStateCreateInfo>(VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO) };
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
        colorBlending.pAttachments = colorBlendAttachments.data();
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
        return colorBlending;
    }

    VkPipelineDynamicStateCreateInfo CreatePipelineDynamicStateCreateInfo(const VkDynamicState* dynamicStates, const uint32_t dynamicStateCount)
    {
        VkPipelineDynamicStateCreateInfo dynamicState{ prev::util::vk::CreateStruct<VkPipelineDynamicStateCreateInfo>(VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO) };
        dynamicState.pDynamicStates = dynamicStates;
        dynamicState.dynamicStateCount = dynamicStateCount;
        return dynamicState;
    }

    VkPipelineTessellationStateCreateInfo CreatePipelineTessellationStateCreateInfo(const uint32_t patchControlPointCount)
    {
        VkPipelineTessellationStateCreateInfo tesselation{ prev::util::vk::CreateStruct<VkPipelineTessellationStateCreateInfo>(VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO) };
        tesselation.patchControlPoints = patchControlPointCount;
        return tesselation;
    }
} // namespace

GraphicsPipelineBuilder::GraphicsPipelineBuilder(const VkDevice device, const shader::Shader& shader, const pass::RenderPass& renderPass)
    : AbstractPipelineBuilder(device, shader)
    , m_renderPass{ renderPass }
{
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetPrimitiveTopology(VkPrimitiveTopology primitiveTopology)
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

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetPatchControlPointCount(uint32_t count)
{
    m_patchControlPointCount = count;
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetPolygonMode(VkPolygonMode mode)
{
    m_polygonMode = mode;
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetCullingMode(VkCullModeFlagBits mode)
{
    m_cullingMode = mode;
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetFrontFace(VkFrontFace frontFace)
{
    m_frontFace = frontFace;
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetSampleShadingEnabled(bool enabled)
{
    m_sampleShadingEnabled = enabled;
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetSampleShadingMinimumFraction(float fraction)
{
    m_sampleShadingMinFraction = fraction;
    return *this;
}

std::unique_ptr<Pipeline> GraphicsPipelineBuilder::Build() const
{
    Validate();

    auto pipelineLayout{ CreatePipelineLayout() };
    auto pipeline{ CreateGraphicsPipeline(pipelineLayout) };

    return std::unique_ptr<Pipeline>(new Pipeline(m_device, pipeline, pipelineLayout));
}

VkPipeline GraphicsPipelineBuilder::CreateGraphicsPipeline(const VkPipelineLayout pipelineLayout) const
{
    const auto vertexInputStateCreateInfo{ CreatePipelineVertexInputStateCreateInfo(m_shader.GetVertexInputBindingDescriptions(), m_shader.GetVertexInputAttributeDescriptions()) };
    const auto inputAssemblyCreateInfo{ CreatePipelineInputAssemblyStateCreateInfo(m_primitiveTopology) };

    const VkExtent2D defaultExtent{ 640, 480 };

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(defaultExtent.width);
    viewport.height = static_cast<float>(defaultExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = defaultExtent;

    const auto viewportStateCreateInfo{ CreatePipelineViewportStateCreateInfo(viewport, scissor) };

    const auto rasterizerStateCreateInfo{ CreatePipelineRasterizationStateCreateInfo(m_polygonMode, m_cullingMode, m_frontFace) };
    const auto multisamplingStateCreateInfo{ CreatePipelineMultisampleStateCreateInfo(m_renderPass.GetSamplesCount(), m_sampleShadingEnabled, m_sampleShadingMinFraction) };
    const auto depthStencilStateCreateInfo{ CreatePipelineDepthStencilStateCreateInfo(m_depthTestEnabled, m_depthWriteEnabled) };

    const auto colorBlendAttachments{ CreatePipelineColorBlendAttachmentStates(m_blendingEnabled, m_additiveBlendingEnabled, static_cast<uint32_t>(m_renderPass.GetColorFormats().size())) };
    const auto colorBlendingStateCreateInfo{ CreatePipelineColorBlendStateCreateInfo(colorBlendAttachments) };

    const VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    const auto dynamicStateCreateInfo{ CreatePipelineDynamicStateCreateInfo(dynamicStates, static_cast<uint32_t>(prev::util::ArraySize(dynamicStates))) };

    const auto tesselationStateCreateInfo{ CreatePipelineTessellationStateCreateInfo(m_patchControlPointCount) };

    VkGraphicsPipelineCreateInfo pipelineInfo{ prev::util::vk::CreateStruct<VkGraphicsPipelineCreateInfo>(VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO) };
    pipelineInfo.stageCount = static_cast<uint32_t>(m_shader.GetShaderStages().size());
    pipelineInfo.pStages = m_shader.GetShaderStages().data();
    pipelineInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    pipelineInfo.pViewportState = &viewportStateCreateInfo;
    pipelineInfo.pRasterizationState = &rasterizerStateCreateInfo;
    pipelineInfo.pMultisampleState = &multisamplingStateCreateInfo;
    pipelineInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    pipelineInfo.pColorBlendState = &colorBlendingStateCreateInfo;
    pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineInfo.pTessellationState = m_patchControlPointCount > 0 ? &tesselationStateCreateInfo : nullptr;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = m_renderPass;

    VkPipeline pipeline;
    VKERRCHECK(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));
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

    if ((m_primitiveTopology != VK_PRIMITIVE_TOPOLOGY_PATCH_LIST && m_patchControlPointCount > 0) || (m_primitiveTopology == VK_PRIMITIVE_TOPOLOGY_PATCH_LIST && m_patchControlPointCount == 0)) {
        throw std::runtime_error("Invalid pipeline configuration: Invalid tesselation configuration - PrimitiveTopology = " + std::to_string(m_primitiveTopology) + " and PatchControlCount = " + std::to_string(m_patchControlPointCount) + ".");
    }

    if (m_sampleShadingEnabled && m_additiveBlendingEnabled && m_renderPass.GetSamplesCount() == VK_SAMPLE_COUNT_1_BIT) {
        throw std::runtime_error("Invalid pipeline configuration: sample shading is enabled while multisampling is disabled - sample shading will be ignored.");
    }
}
} // namespace prev::render::pipeline
