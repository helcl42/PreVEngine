#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include <RenderPass.h>
#include <Shader.h>
#include <Utils.h>

using namespace PreVEngine;

class IGraphicsPipeline {
public:
    virtual VkPipeline Init() = 0;

    virtual void ShutDown() = 0;

    virtual VkPipelineLayout GetLayout() const = 0;

    virtual operator VkPipeline() const = 0;

public:
    virtual ~IGraphicsPipeline() {}
};

class AbstractGraphicsPipeline : public IGraphicsPipeline {
protected:
    const VkDevice m_device;

    const VkRenderPass m_renderPass;

    const Shader& m_shaders;

    VkPipeline m_graphicsPipeline;

    VkPipelineLayout m_pipelineLayout;

protected:
    AbstractGraphicsPipeline(const VkDevice device, const VkRenderPass renderpass, const PreVEngine::Shader& shaders)
        : m_device(device)
        , m_renderPass(renderpass)
        , m_shaders(shaders)
        , m_graphicsPipeline(VK_NULL_HANDLE)
        , m_pipelineLayout(VK_NULL_HANDLE)
    {
    }

    virtual ~AbstractGraphicsPipeline()
    {
        ShutDown();
    }

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

        if (m_graphicsPipeline) {
            vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
            m_graphicsPipeline = VK_NULL_HANDLE;
        }
    }

public:
    VkPipelineLayout GetLayout() const override
    {
        return m_pipelineLayout;
    }

    operator VkPipeline() const override
    {
        return m_graphicsPipeline;
    }
};

class VkPipelineFactory {
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
        vertexInputState.vertexBindingDescriptionCount = 1;
        vertexInputState.pVertexBindingDescriptions = shader.GetVertexInputBindingDescription();
        vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(shader.GetVertexInputAttributeDewcriptions().size());
        vertexInputState.pVertexAttributeDescriptions = shader.GetVertexInputAttributeDewcriptions().data();

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

    void CreateDefaultPipeline(const VkDevice& device, const VkRenderPass& renderPass, const Shader& shader, const bool depthTestEnabled, const bool fillMode, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const
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
        vertexInputState.vertexBindingDescriptionCount = 1;
        vertexInputState.pVertexBindingDescriptions = shader.GetVertexInputBindingDescription();
        vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(shader.GetVertexInputAttributeDewcriptions().size());
        vertexInputState.pVertexAttributeDescriptions = shader.GetVertexInputAttributeDewcriptions().data();

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
        if (depthTestEnabled) {
            depthStencilState.depthTestEnable = VK_TRUE;
            depthStencilState.depthWriteEnable = VK_TRUE;
        } else {
            depthStencilState.depthTestEnable = VK_FALSE;
            depthStencilState.depthWriteEnable = VK_FALSE;
        }
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
        vertexInputState.vertexBindingDescriptionCount = 1;
        vertexInputState.pVertexBindingDescriptions = shader.GetVertexInputBindingDescription();
        vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(shader.GetVertexInputAttributeDewcriptions().size());
        vertexInputState.pVertexAttributeDescriptions = shader.GetVertexInputAttributeDewcriptions().data();

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
};

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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateShadowsPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
    }
};

class NormalMappedShadowsShader final : public Shader {
public:
    NormalMappedShadowsShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~NormalMappedShadowsShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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

class NormalMappedShadowsPipeline final : public AbstractGraphicsPipeline {
public:
    NormalMappedShadowsPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~NormalMappedShadowsPipeline() = default;

public:
    VkPipeline Init() override
    {
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateShadowsPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateShadowsPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
    }
};

class TerrainNormalMappedShadowsShader final : public Shader {
public:
    TerrainNormalMappedShadowsShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~TerrainNormalMappedShadowsShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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

class TerrainNormalMappedShadowsPipeline final : public AbstractGraphicsPipeline {
public:
    TerrainNormalMappedShadowsPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~TerrainNormalMappedShadowsPipeline() = default;

public:
    VkPipeline Init() override
    {
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateShadowsPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4 }), VK_VERTEX_INPUT_RATE_VERTEX);

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

class AnimatedNormalMappedShadowsPipeline final : public AbstractGraphicsPipeline {
public:
    AnimatedNormalMappedShadowsPipeline(const VkDevice device, const VkRenderPass renderpass, const Shader& shaders)
        : AbstractGraphicsPipeline(device, renderpass, shaders)
    {
    }

    ~AnimatedNormalMappedShadowsPipeline() = default;

public:
    VkPipeline Init() override
    {
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateShadowsPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
    }
};

class AnimatedNormalMappedShadowsShader final : public Shader {
public:
    AnimatedNormalMappedShadowsShader(const VkDevice device)
        : Shader(device)
    {
    }

    ~AnimatedNormalMappedShadowsShader() = default;

private:
    void InitVertexInputs() override
    {
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateShadowsPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
    }
};

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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, true, true, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, true, true, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, true, true, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
    }
};

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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::IVEC4, VertexLayoutComponent::VEC4 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, true, true, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
    }
};

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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateDebugPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
    }
};

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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC2 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, false, true, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
    }
};

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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, true, true, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, true, true, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
    }
};

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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, false, true, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
    }
};

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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, true, true, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
    }
};

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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC2 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, true, true, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3, VertexLayoutComponent::VEC2, VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateDebugPipeline(m_device, m_renderPass, m_shaders, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
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
        m_inputBindingDescription = VkUtils::CreateVertexInputBindingDescription(0, VertexLayout::GetComponentsSize({ VertexLayoutComponent::VEC3 }), VK_VERTEX_INPUT_RATE_VERTEX);

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
        VkPipelineFactory pipelineFactory{};
        pipelineFactory.CreateDefaultPipeline(m_device, m_renderPass, m_shaders, true, false, m_pipelineLayout, m_graphicsPipeline);
        return m_graphicsPipeline;
    }
};

#endif