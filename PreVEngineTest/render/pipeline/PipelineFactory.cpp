#include "PipelineFactory.h"

namespace prev_test::render::pipeline {
void PipelineFactory::CreateShadowsPipeline(const VkDevice& device, const prev::render::shader::Shader& shader, const prev::render::pass::RenderPass& renderPass, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const
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
    rasterizer.depthBiasEnable = VK_TRUE;
    rasterizer.depthBiasConstantFactor = 1.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 1.75f;

    VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    if (renderPass.GetSamplesCount() > VK_SAMPLE_COUNT_1_BIT) {
        multisampling.sampleShadingEnable = VK_TRUE;
        multisampling.rasterizationSamples = renderPass.GetSamplesCount();
        multisampling.minSampleShading = 0.2f;
    } else {
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = renderPass.GetSamplesCount();
    }

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
    dynamicState.dynamicStateCount = static_cast<uint32_t>(prev::util::ArraySize(dynamicStates));

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
    pipelineInfo.renderPass = renderPass.GetNativeHandle();

    VKERRCHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &outPipeline));
}

void PipelineFactory::CreateDefaultPipeline(const VkDevice& device, const prev::render::shader::Shader& shader, const prev::render::pass::RenderPass& renderPass, const VkPrimitiveTopology topology, const bool depthTestEnabled, const bool depthWriteEnabled, const bool fillMode, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const
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
    rasterizer.polygonMode = fillMode ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    if (renderPass.GetSamplesCount() > VK_SAMPLE_COUNT_1_BIT) {
        multisampling.sampleShadingEnable = VK_TRUE;
        multisampling.rasterizationSamples = renderPass.GetSamplesCount();
        multisampling.minSampleShading = 0.2f;
    } else {
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = renderPass.GetSamplesCount();
    }

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
    dynamicState.dynamicStateCount = static_cast<uint32_t>(prev::util::ArraySize(dynamicStates));

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
    pipelineInfo.renderPass = renderPass.GetNativeHandle();

    VKERRCHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &outPipeline));
}

void PipelineFactory::CreateParticlesPipeline(const VkDevice& device, const prev::render::shader::Shader& shader, const prev::render::pass::RenderPass& renderPass, const VkPrimitiveTopology topology, const bool depthTestEnabled, const bool depthWriteEnabled, const bool fillMode, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const
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
    rasterizer.polygonMode = fillMode ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    if (renderPass.GetSamplesCount() > VK_SAMPLE_COUNT_1_BIT) {
        multisampling.sampleShadingEnable = VK_TRUE;
        multisampling.rasterizationSamples = renderPass.GetSamplesCount();
        multisampling.minSampleShading = 0.2f;
    } else {
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = renderPass.GetSamplesCount();
    }

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
    dynamicState.dynamicStateCount = static_cast<uint32_t>(prev::util::ArraySize(dynamicStates));

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
    pipelineInfo.renderPass = renderPass.GetNativeHandle();

    VKERRCHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &outPipeline));
}

void PipelineFactory::CreateDebugPipeline(const VkDevice& device, const prev::render::shader::Shader& shader, const prev::render::pass::RenderPass& renderPass, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const
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
    if (renderPass.GetSamplesCount() > VK_SAMPLE_COUNT_1_BIT) {
        multisampling.sampleShadingEnable = VK_TRUE;
        multisampling.rasterizationSamples = renderPass.GetSamplesCount();
        multisampling.minSampleShading = 0.2f;
    } else {
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = renderPass.GetSamplesCount();
    }

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
    dynamicState.dynamicStateCount = static_cast<uint32_t>(prev::util::ArraySize(dynamicStates));

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
    pipelineInfo.renderPass = renderPass.GetNativeHandle();

    VKERRCHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &outPipeline));
}

void PipelineFactory::CreateDefaultComputePipeline(const VkDevice& device, const prev::render::shader::Shader& shader, VkPipelineLayout& outPipelineLayout, VkPipeline& outPipeline) const
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
} // namespace prev_test::render::pipeline