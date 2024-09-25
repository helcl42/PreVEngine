#include "PipelineBuilder.h"

#include "../../common/Logger.h"
#include "../../util/Utils.h"

#include <vector>

namespace prev::render::pipeline {
namespace {
    VkPipelineLayout CreatePipelineLayout(const VkDevice device, const shader::Shader& shader)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = shader.GetDescriptorSetLayout();
        pipelineLayoutInfo.pPushConstantRanges = shader.GetPushConstantsRanges().data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(shader.GetPushConstantsRanges().size());

        VkPipelineLayout pipelineLayout;
        VKERRCHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout));
        return pipelineLayout;
    }

    VkPipeline CreateComputePipeline(const VkDevice device, const VkPipelineLayout pipelineLayout, const shader::Shader& shader)
    {
        VkComputePipelineCreateInfo computePipelineCreateInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
        computePipelineCreateInfo.layout = pipelineLayout;
        computePipelineCreateInfo.flags = 0;
        computePipelineCreateInfo.stage = shader.GetShaderStages().at(0);

        VkPipeline pipeline;
        VKERRCHECK(vkCreateComputePipelines(device, nullptr, 1, &computePipelineCreateInfo, nullptr, &pipeline));
        return pipeline;
    }

    VkPipeline CreateGraphicsPipeline(const VkDevice& device, const shader::Shader& shader, const pass::RenderPass& renderPass, const VkPipelineLayout pipelineLayout, const VkPrimitiveTopology topology, const bool depthTestEnabled, const bool depthWriteEnabled, const bool blendingEnabled, const bool additiveBlendingEnabled, const VkPolygonMode polygonMode, const VkCullModeFlagBits cullingMode, const VkFrontFace frontFace, const bool sampleShadingEnabled, const float minSampleShadingFraction, const uint32_t patchPointCount)
    {
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssembly.topology = topology;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineVertexInputStateCreateInfo vertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(shader.GetVertexInputBindingDescriptions().size());
        vertexInputState.pVertexBindingDescriptions = shader.GetVertexInputBindingDescriptions().data();
        vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(shader.GetVertexInputAttributeDescriptions().size());
        vertexInputState.pVertexAttributeDescriptions = shader.GetVertexInputAttributeDescriptions().data();

        const VkExtent2D initialExtent{ 640, 480 };

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(initialExtent.width);
        viewport.height = static_cast<float>(initialExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = initialExtent;

        VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizer.depthClampEnable = VK_TRUE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = polygonMode;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = cullingMode;
        rasterizer.frontFace = frontFace;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        if (renderPass.GetSamplesCount() > VK_SAMPLE_COUNT_1_BIT) {
            multisampling.sampleShadingEnable = sampleShadingEnabled;
            multisampling.rasterizationSamples = renderPass.GetSamplesCount();
            multisampling.minSampleShading = minSampleShadingFraction;
        } else {
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = renderPass.GetSamplesCount();
        }

        VkPipelineDepthStencilStateCreateInfo depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depthStencilState.depthTestEnable = depthTestEnabled ? VK_TRUE : VK_FALSE;
        depthStencilState.depthWriteEnable = depthWriteEnabled ? VK_TRUE : VK_FALSE;
        if constexpr (REVERSE_DEPTH) {
            depthStencilState.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
        } else {
            depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        }
        depthStencilState.depthBoundsTestEnable = VK_FALSE;
        depthStencilState.stencilTestEnable = VK_FALSE;

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(renderPass.GetColorFormats().size());

        for (uint32_t i = 0; i < static_cast<uint32_t>(renderPass.GetColorFormats().size()); ++i) {
            VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
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

        VkPipelineColorBlendStateCreateInfo colorBlending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
        colorBlending.pAttachments = colorBlendAttachments.data();
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.pDynamicStates = dynamicStates;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(prev::util::ArraySize(dynamicStates));

        VkPipelineTessellationStateCreateInfo tesselationStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
        tesselationStateCreateInfo.patchControlPoints = patchPointCount;

        VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipelineInfo.stageCount = static_cast<uint32_t>(shader.GetShaderStages().size());
        pipelineInfo.pStages = shader.GetShaderStages().data();
        pipelineInfo.pVertexInputState = &vertexInputState;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencilState;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.pTessellationState = patchPointCount > 0 ? &tesselationStateCreateInfo : nullptr;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;

        VkPipeline pipeline;
        VKERRCHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));
        return pipeline;
    }
} // namespace

ComputePipelineBuilder::ComputePipelineBuilder(const VkDevice device, const shader::Shader& shader)
    : m_device{ device }
    , m_shader{ shader }
{
}

std::unique_ptr<Pipeline> ComputePipelineBuilder::Build() const
{
    auto pipelineLayout{ CreatePipelineLayout(m_device, m_shader) };
    auto pipeline{ CreateComputePipeline(m_device, pipelineLayout, m_shader) };
    return std::make_unique<Pipeline>(m_device, pipeline, pipelineLayout);
}

GraphicsPipelineBuilder::GraphicsPipelineBuilder(const VkDevice device, const shader::Shader& shader, const pass::RenderPass& renderPass)
    : m_device{ device }
    , m_shader{ shader }
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
    if (!m_depthTestEnabled && m_depthWriteEnabled) {
        LOGW("Invalid pipeline configuration: Depth test is disabled but depth write enabled - depth write value will be ignored.");
    }

    if (!m_blendingEnabled && m_additiveBlendingEnabled) {
        LOGW("Invalid pipeline configuration: Blending is disabled and additive blending enabled - additive blending will be ignored.");
    }

    if ((m_primitiveTopology != VK_PRIMITIVE_TOPOLOGY_PATCH_LIST && m_patchControlPointCount > 0) || (m_primitiveTopology == VK_PRIMITIVE_TOPOLOGY_PATCH_LIST && m_patchControlPointCount == 0)) {
        LOGW("Invalid pipeline configuration: Invalid tesselation configuration - PrimitiveTopology = %d and PatchControlCount = %d.", m_primitiveTopology, m_patchControlPointCount);
    }

    if (m_sampleShadingEnabled && m_additiveBlendingEnabled && m_renderPass.GetSamplesCount() == VK_SAMPLE_COUNT_1_BIT) {
        LOGW("Invalid pipeline configuration: sample shading is enabled while multisampling is disabled - sample shading will be ignored.");
    }

    auto pipelineLayout{ CreatePipelineLayout(m_device, m_shader) };
    auto pipeline{ CreateGraphicsPipeline(m_device, m_shader, m_renderPass, pipelineLayout, m_primitiveTopology, m_depthTestEnabled, m_depthWriteEnabled, m_blendingEnabled, m_additiveBlendingEnabled, m_polygonMode, m_cullingMode, m_frontFace, m_sampleShadingEnabled, m_sampleShadingMinFraction, m_patchControlPointCount) };
    return std::make_unique<Pipeline>(m_device, pipeline, pipelineLayout);
}

} // namespace prev::render::pipeline
