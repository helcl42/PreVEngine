#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include <RenderPass.h>
#include <Shader.h>
#include <Utils.h>

using namespace PreVEngine;

class IPipeline {
public:
    virtual VkPipeline Init() = 0;

    virtual void ShutDown() = 0;

    virtual VkPipelineLayout GetLayout() const = 0;

    virtual operator VkPipeline() const = 0;

public:
    virtual ~IPipeline() {}
};

class AbstractPipeline : public IPipeline {
public:
    void ShutDown() override
    {
        if (m_device) {
            vkDeviceWaitIdle(m_device);
        }

        if (m_pipelineLayout) {
            vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
            m_pipelineLayout = VK_NULL_HANDLE;
        }

        if (m_pipeline) {
            vkDestroyPipeline(m_device, m_pipeline, nullptr);
            m_pipeline = VK_NULL_HANDLE;
        }
    }

    VkPipelineLayout GetLayout() const override
    {
        return m_pipelineLayout;
    }

    operator VkPipeline() const override
    {
        return m_pipeline;
    }

protected:
    AbstractPipeline(const VkDevice device, const Shader& shaders)
        : m_device(device)
        , m_shaders(shaders)
        , m_pipeline(VK_NULL_HANDLE)
        , m_pipelineLayout(VK_NULL_HANDLE)
    {
    }

    ~AbstractPipeline() = default;

protected:
    const VkDevice m_device;

    const Shader& m_shaders;

    VkPipeline m_pipeline;

    VkPipelineLayout m_pipelineLayout;
};


class AbstractGraphicsPipeline : public AbstractPipeline {
protected:
    AbstractGraphicsPipeline(const VkDevice device, const VkRenderPass renderpass, const PreVEngine::Shader& shaders)
        : AbstractPipeline(device, shaders)
        , m_renderPass(renderpass)
    {
    }

    virtual ~AbstractGraphicsPipeline() = default;

protected:
    const VkRenderPass m_renderPass;
};

class AbstractComputePipeline : public AbstractPipeline {
protected:
    AbstractComputePipeline(const VkDevice device, const Shader& shaders)
        : AbstractPipeline(device, shaders)
    {
    }

    ~AbstractComputePipeline() = default;
};

class PipelineFactory {
public:
    void CreateShadowsPipeline(const VkDevice& device, const VkRenderPass& renderPass, const Shader& shader, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const
    {
        // Pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = shader.GetDescriptorSetLayout();
        pipelineLayoutInfo.pPushConstantRanges = shader.GetPushConstantsRanges().data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(shader.GetPushConstantsRanges().size());
        VKERRCHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &outPipelineLayout));

        // Pipeline
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depthStencilState.depthTestEnable = VK_TRUE;
        depthStencilState.depthWriteEnable = VK_TRUE;
        depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilState.depthBoundsTestEnable = VK_FALSE;
        depthStencilState.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 0;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.pDynamicStates = dynamicStates;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(ArraySize(dynamicStates));

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
        pipelineInfo.layout = outPipelineLayout;
        pipelineInfo.renderPass = renderPass;

        VKERRCHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &outPipeline));
    }

    void CreateDefaultPipeline(const VkDevice& device, const VkRenderPass& renderPass, const Shader& shader, const VkPrimitiveTopology topology, const bool depthTestEnabled, const bool depthWriteEnabled, const bool fillMode, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const
    {
        // Pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = shader.GetDescriptorSetLayout();
        pipelineLayoutInfo.pPushConstantRanges = shader.GetPushConstantsRanges().data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(shader.GetPushConstantsRanges().size());
        VKERRCHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &outPipelineLayout));

        // Pipeline
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
        if (fillMode) {
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        } else {
            rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
        }        
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depthStencilState.depthTestEnable = depthTestEnabled ? VK_TRUE : VK_FALSE;
        depthStencilState.depthWriteEnable = depthWriteEnabled ? VK_TRUE : VK_FALSE;
        depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilState.depthBoundsTestEnable = VK_FALSE;
        depthStencilState.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.pDynamicStates = dynamicStates;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(ArraySize(dynamicStates));

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
        pipelineInfo.layout = outPipelineLayout;
        pipelineInfo.renderPass = renderPass;

        VKERRCHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &outPipeline));
    }

    void CreateParticlesPipeline(const VkDevice& device, const VkRenderPass& renderPass, const Shader& shader, const VkPrimitiveTopology topology, const bool depthTestEnabled, const bool depthWriteEnabled, const bool fillMode, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const
    {
        // Pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = shader.GetDescriptorSetLayout();
        pipelineLayoutInfo.pPushConstantRanges = shader.GetPushConstantsRanges().data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(shader.GetPushConstantsRanges().size());
        VKERRCHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &outPipelineLayout));

        // Pipeline
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
        if (fillMode) {
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        } else {
            rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
        }
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depthStencilState.depthTestEnable = depthTestEnabled ? VK_TRUE : VK_FALSE;
        depthStencilState.depthWriteEnable = depthWriteEnabled ? VK_TRUE : VK_FALSE;
        depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilState.depthBoundsTestEnable = VK_FALSE;
        depthStencilState.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.pDynamicStates = dynamicStates;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(ArraySize(dynamicStates));

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
        pipelineInfo.layout = outPipelineLayout;
        pipelineInfo.renderPass = renderPass;

        VKERRCHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &outPipeline));
    }

    void CreateDebugPipeline(const VkDevice& device, const VkRenderPass& renderPass, const Shader& shader, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const
    {
        // Pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = shader.GetDescriptorSetLayout();
        pipelineLayoutInfo.pPushConstantRanges = shader.GetPushConstantsRanges().data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(shader.GetPushConstantsRanges().size());
        VKERRCHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &outPipelineLayout));

        // Pipeline
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depthStencilState.depthTestEnable = VK_FALSE;
        depthStencilState.depthWriteEnable = VK_FALSE;
        depthStencilState.depthCompareOp = VK_COMPARE_OP_ALWAYS;
        depthStencilState.depthBoundsTestEnable = VK_FALSE;
        depthStencilState.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.pDynamicStates = dynamicStates;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(ArraySize(dynamicStates));

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
        pipelineInfo.layout = outPipelineLayout;
        pipelineInfo.renderPass = renderPass;

        VKERRCHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &outPipeline));
    }

    void CreateDefaultComputePipeline(const VkDevice& device, const Shader& shader, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = shader.GetDescriptorSetLayout();
        pipelineLayoutInfo.pPushConstantRanges = shader.GetPushConstantsRanges().data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(shader.GetPushConstantsRanges().size());
        VKERRCHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &outPipelineLayout));

        VkComputePipelineCreateInfo computePipelineCreateInfo{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
        computePipelineCreateInfo.layout = outPipelineLayout;
        computePipelineCreateInfo.flags = 0;
        computePipelineCreateInfo.stage = shader.GetShaderStages().at(0);

        VKERRCHECK(vkCreateComputePipelines(device, nullptr, 1, &computePipelineCreateInfo, nullptr, &outPipeline));
    }
};

//////////////////////////////////////////// SHADDWS ////////////////////////////////////////////

class DefaultShadowsShader final : public Shader {
public:
    DefaultShadowsShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~DefaultShadowsShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("ubo", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class DefaultShadowsPipeline final : public AbstractGraphicsPipeline {
public:
    DefaultShadowsPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~DefaultShadowsPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateShadowsPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class BumpMappedShadowsShader final : public Shader {
public:
    BumpMappedShadowsShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~BumpMappedShadowsShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = {
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 })),
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("ubo", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class BumpMappedShadowsPipeline final : public AbstractGraphicsPipeline {
public:
    BumpMappedShadowsPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~BumpMappedShadowsPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateShadowsPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class TerrainShadowsShader final : public Shader {
public:
    TerrainShadowsShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~TerrainShadowsShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = {
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("ubo", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class TerrainShadowsPipeline final : public AbstractGraphicsPipeline {
public:
    TerrainShadowsPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~TerrainShadowsPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateShadowsPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class TerrainBumpMappedShadowsShader final : public Shader {
public:
    TerrainBumpMappedShadowsShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~TerrainBumpMappedShadowsShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 })),
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("ubo", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class TerrainBumpMappedShadowsPipeline final : public AbstractGraphicsPipeline {
public:
    TerrainBumpMappedShadowsPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~TerrainBumpMappedShadowsPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateShadowsPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class AnimatedShadowsShader final : public Shader {
public:
    AnimatedShadowsShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~AnimatedShadowsShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = {
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4 }), VK_VERTEX_INPUT_RATE_VERTEX)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32A32_SINT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("ubo", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class AnimatedShadowsPipeline final : public AbstractGraphicsPipeline {
public:
    AnimatedShadowsPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~AnimatedShadowsPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateShadowsPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class AnimatedBumplMappedShadowsShader final : public Shader {
public:
    AnimatedBumplMappedShadowsShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~AnimatedBumplMappedShadowsShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = {
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32A32_SINT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 5, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 6, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("ubo", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class AnimatedBumpMappedShadowsPipeline final : public AbstractGraphicsPipeline {
public:
    AnimatedBumpMappedShadowsPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~AnimatedBumpMappedShadowsPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateShadowsPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

//////////////////////////////////////////// DEFAULT ////////////////////////////////////////////

class DefaultShader final : public Shader {
public:
    DefaultShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~DefaultShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = {
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("depthSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class DefaultPipeline final : public AbstractGraphicsPipeline {
public:
    DefaultPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~DefaultPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class NormalMappedShader final : public Shader {
public:
    NormalMappedShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~NormalMappedShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("normalSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("depthSampler", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class NormalMappedPipeline final : public AbstractGraphicsPipeline {
public:
    NormalMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~NormalMappedPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class ParallaxMappedShader final : public Shader {
public:
    ParallaxMappedShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~ParallaxMappedShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("normalSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("heightSampler", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("depthSampler", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class ParallaxMappedPipeline final : public AbstractGraphicsPipeline {
public:
    ParallaxMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~ParallaxMappedPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class ConeStepMappedShader final : public Shader {
public:
    ConeStepMappedShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~ConeStepMappedShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("normalSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("heightSampler", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("depthSampler", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class ConeStepMappedPipeline final : public AbstractGraphicsPipeline {
public:
    ConeStepMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~ConeStepMappedPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

//////////////////////////////////////////// ANIMATION ////////////////////////////////////////////

class AnimationShader final : public Shader {
public:
    AnimationShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~AnimationShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = {
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4 }), VK_VERTEX_INPUT_RATE_VERTEX)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32A32_SINT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("depthSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class AnimationPipeline final : public AbstractGraphicsPipeline {
public:
    AnimationPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~AnimationPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class AnimationNormalMappedShader final : public Shader {
public:
    AnimationNormalMappedShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~AnimationNormalMappedShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = {
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32A32_SINT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 5, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 6, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("normalSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("depthSampler", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class AnimationNormalMappedPipeline final : public AbstractGraphicsPipeline {
public:
    AnimationNormalMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~AnimationNormalMappedPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class AnimationParallaxMappedShader final : public Shader {
public:
    AnimationParallaxMappedShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~AnimationParallaxMappedShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = {
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32A32_SINT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 5, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 6, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("normalSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("heightSampler", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("depthSampler", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class AnimationParallaxMappedPipeline final : public AbstractGraphicsPipeline {
public:
    AnimationParallaxMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~AnimationParallaxMappedPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class AnimationConeStepMappedShader final : public Shader {
public:
    AnimationConeStepMappedShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~AnimationConeStepMappedShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32A32_SINT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 5, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 6, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("normalSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("heightSampler", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("depthSampler", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class AnimationConeStepMappedPipeline final : public AbstractGraphicsPipeline {
public:
    AnimationConeStepMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~AnimationConeStepMappedPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

//////////////////////////////////////////// FONT ////////////////////////////////////////////

class FonttShader final : public Shader {
public:
    FonttShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~FonttShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC2 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class FontPipeline final : public AbstractGraphicsPipeline {
public:
    FontPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~FontPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, false, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

//////////////////////////////////////////// TERRAIN ////////////////////////////////////////////

class TerrainShader final : public Shader {
public:
    TerrainShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~TerrainShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
        };
    }
    
    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT);

        AddDescriptorSet("depthSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class TerrainPipeline final : public AbstractGraphicsPipeline {
public:
    TerrainPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~TerrainPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class TerrainNormalMappedShader final : public Shader {
public:
    TerrainNormalMappedShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~TerrainNormalMappedShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 })),
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("normalSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("depthSampler", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class TerrainNormalMappedPipeline final : public AbstractGraphicsPipeline {
public:
    TerrainNormalMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~TerrainNormalMappedPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class TerrainParallaxMappedShader final : public Shader {
public:
    TerrainParallaxMappedShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~TerrainParallaxMappedShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 })),
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("normalSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("heightSampler", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("depthSampler", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class TerrainParallaxMappedPipeline final : public AbstractGraphicsPipeline {
public:
    TerrainParallaxMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~TerrainParallaxMappedPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true,  true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class TerrainConeStepMappedShader final : public Shader {
public:
    TerrainConeStepMappedShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~TerrainConeStepMappedShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 })),
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("normalSampler", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("heightSampler", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("depthSampler", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class TerrainConeStepMappedPipeline final : public AbstractGraphicsPipeline {
public:
    TerrainConeStepMappedPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~TerrainConeStepMappedPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

//////////////////////////////////////////// SKY BOX ////////////////////////////////////////////

class SkyBoxShader final : public Shader {
public:
    SkyBoxShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~SkyBoxShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = {
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("cubeMap1", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class SkyBoxPipeline final : public AbstractGraphicsPipeline {
public:
    SkyBoxPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~SkyBoxPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, false, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

//////////////////////////////////////////// WATER ////////////////////////////////////////////

class WaterShader final : public Shader {
public:
    WaterShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~WaterShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

        AddDescriptorSet("depthSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("reflectionTexture", 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("refractionTexture", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("dudvMapTexture", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("normalMapTexture", 6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        AddDescriptorSet("depthMapTexture", 7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class WaterPipeline final : public AbstractGraphicsPipeline {
public:
    WaterPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~WaterPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

//////////////////////////////////////////// FLARES ////////////////////////////////////////////

class FlareShader final : public Shader {
public:
    FlareShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~FlareShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, 0)
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class FlarePipeline final : public AbstractGraphicsPipeline {
public:
    FlarePipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~FlarePipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

//////////////////////////////////////////// PARTICLES ////////////////////////////////////////////

class ParticlesShader final : public Shader {
public:
    ParticlesShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~ParticlesShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = {
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX),
            VkUtils::CreateVertexInputBindingDescription(1, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC2, VertexLayoutComponent::FLOAT }), VK_VERTEX_INPUT_RATE_INSTANCE)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),

            // mat4 -> transformation matrix
            // vec4 -> texture offset curr, next stages
            // float -> blend factor of curr, next texture stages
            VkUtils::CreateVertexInputAttributeDescription(1, 3, VK_FORMAT_R32G32B32A32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(1, 4, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4 })),
            VkUtils::CreateVertexInputAttributeDescription(1, 5, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4 })),
            VkUtils::CreateVertexInputAttributeDescription(1, 6, VK_FORMAT_R32G32B32A32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4 })),

            VkUtils::CreateVertexInputAttributeDescription(1, 7, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4 })),
            VkUtils::CreateVertexInputAttributeDescription(1, 8, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC2 })),

            VkUtils::CreateVertexInputAttributeDescription(1, 9, VK_FORMAT_R32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC2 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

        AddDescriptorSet("textureSampler", 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class ParticlesPipeline final : public AbstractGraphicsPipeline {
public:
    ParticlesPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~ParticlesPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateParticlesPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, false, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

//////////////////////////////////////////// COMPUTE ////////////////////////////////////////////

class DummyComputeShader final : public Shader {
public:
    DummyComputeShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~DummyComputeShader() = default;

private:
    void InitVertexInputs() override
    {
    }

    void InitDescriptorSets() override
    {
        AddDescriptorSet("dataBuffer", 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class DummyComputePipeline final : public AbstractComputePipeline {
public:
    DummyComputePipeline(const VkDevice device, const Shader& shaders)
        : AbstractComputePipeline(device, shaders)
    {
    }

    ~DummyComputePipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultComputePipeline(m_device, m_shaders, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

//////////////////////////////////////////// SKY ////////////////////////////////////////////

class WeatherComputeShader final : public Shader {
public:
    WeatherComputeShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~WeatherComputeShader() = default;

private:
    void InitVertexInputs() override
    {
    }

    void InitDescriptorSets() override
    {
        AddDescriptorSet("uboCS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT);
        AddDescriptorSet("outWeatherTexture", 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class WeatherComputePipeline final : public AbstractComputePipeline {
public:
    WeatherComputePipeline(const VkDevice device, const Shader& shaders)
        : AbstractComputePipeline(device, shaders)
    {
    }

    ~WeatherComputePipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultComputePipeline(m_device, m_shaders, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class PerlinWorleyComputeShader final : public Shader {
public:
    PerlinWorleyComputeShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~PerlinWorleyComputeShader() = default;

private:
    void InitVertexInputs() override
    {
    }

    void InitDescriptorSets() override
    {
        AddDescriptorSet("outVolumeTexture", 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class PerlinWorleyComputePipeline final : public AbstractComputePipeline {
public:
    PerlinWorleyComputePipeline(const VkDevice device, const Shader& shaders)
        : AbstractComputePipeline(device, shaders)
    {
    }

    ~PerlinWorleyComputePipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultComputePipeline(m_device, m_shaders, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class WorleyComputeShader final : public Shader {
public:
    WorleyComputeShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~WorleyComputeShader() = default;

private:
    void InitVertexInputs() override
    {
    }

    void InitDescriptorSets() override
    {
        AddDescriptorSet("outVolumeTexture", 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class WorleyComputePipeline final : public AbstractComputePipeline {
public:
    WorleyComputePipeline(const VkDevice device, const Shader& shaders)
        : AbstractComputePipeline(device, shaders)
    {
    }

    ~WorleyComputePipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultComputePipeline(m_device, m_shaders, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class SkyShader final : public Shader {
public:
    SkyShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~SkyShader() = default;

private:
    void InitVertexInputs() override
    {
    }

    void InitDescriptorSets() override
    {
        AddDescriptorSet("outFragColor", 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
        AddDescriptorSet("outBloom", 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
        AddDescriptorSet("outAlphaness", 2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);
        AddDescriptorSet("outCloudDistance", 3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT);

        AddDescriptorSet("perlinNoiseTex", 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT);
        AddDescriptorSet("weatherTex", 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_COMPUTE_BIT);

        AddDescriptorSet("uboCS", 6, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class SkyPipeline final : public AbstractComputePipeline {
public:
    SkyPipeline(const VkDevice device, const Shader& shaders)
        : AbstractComputePipeline(device, shaders)
    {
    }

    ~SkyPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultComputePipeline(m_device, m_shaders, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

//////////////////////////////////////////// Screen Space ////////////////////////////////////////////

class ScreenSpaceShader final : public Shader {
public:
    ScreenSpaceShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~ScreenSpaceShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = {
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX),
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 })),
        };
    }

    void InitDescriptorSets() override
    {
        AddDescriptorSet("image", 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class ScreenSpacePipeline final : public AbstractGraphicsPipeline {
public:
    ScreenSpacePipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~ScreenSpacePipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, false, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

//////////////////////////////////////////// DEBUG ////////////////////////////////////////////

class ShadowMapDebugShader final : public Shader {
public:
    ShadowMapDebugShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~ShadowMapDebugShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX)
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
        };
    }

    void InitDescriptorSets() override
    {
        AddDescriptorSet("depthSampler", 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
        AddPushConstantBlock(VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float) * 3);
    }
};

class ShadowMapDebugPipeline final : public AbstractGraphicsPipeline {
public:
    ShadowMapDebugPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~ShadowMapDebugPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDebugPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class TextureDebugShader final : public Shader {
public:
    TextureDebugShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~TextureDebugShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
        };
    }

    void InitDescriptorSets() override
    {
        AddDescriptorSet("imageSampler", 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
        AddPushConstantBlock(VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float) * 3);
    }
};

class TextureDebugPipeline final : public AbstractGraphicsPipeline {
public:
    TextureDebugPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~TextureDebugPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDebugPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class BoundingVolumeDebugShader final : public Shader {
public:
    BoundingVolumeDebugShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~BoundingVolumeDebugShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class BoundingVolumeDebugPipeline final : public AbstractGraphicsPipeline {
public:
    BoundingVolumeDebugPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~BoundingVolumeDebugPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, false, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class SelectionDebugShader final : public Shader {
public:
    SelectionDebugShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~SelectionDebugShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = {
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
            VkUtils::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 })),
            VkUtils::CreateVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2 }))
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class SelectionDebugPipeline final : public AbstractGraphicsPipeline {
public:
    SelectionDebugPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~SelectionDebugPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, false, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

class RayCastDebugShader final : public Shader {
public:
    RayCastDebugShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~RayCastDebugShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescriptions = { 
            VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX) 
        };

        m_inputAttributeDescriptions = {
            VkUtils::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
        };
    }

    void InitDescriptorSets() override
    {
        // vertex shader
        AddDescriptorSet("uboVS", 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);

        // geometry shader
        AddDescriptorSet("uboGS", 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_GEOMETRY_BIT);

        // fragment shader
        AddDescriptorSet("uboFS", 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    void InitPushConstantsBlocks() override
    {
    }
};

class RayCastDebugPipeline final : public AbstractGraphicsPipeline {
public:
    RayCastDebugPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~RayCastDebugPipeline() = default;

public:
    VkPipeline Init() override
    {
        PipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, VK_PRIMITIVE_TOPOLOGY_POINT_LIST, true, true, true, m_pipelineLayout, m_pipeline);
        return m_pipeline;
    }
};

#endif