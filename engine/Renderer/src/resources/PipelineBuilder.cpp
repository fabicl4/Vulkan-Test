#include <renderer/resources/PipelineBuilder.h>

namespace renderer {

PipelineConfigInfo PipelineBuilder::defaultPipelineConfigInfo() {
    PipelineConfigInfo configInfo {};

    configInfo.vertShaderStageInfo = {};
    configInfo.fragShaderStageInfo = {};

    configInfo.vertexInputInfo = {};
    configInfo.vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    configInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
    configInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;
    configInfo.vertexInputInfo.pVertexBindingDescriptions = nullptr;
    configInfo.vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    configInfo.inputAssemblyInfo = {};
    configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    configInfo.rasterizationInfo = {};
    configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    configInfo.rasterizationInfo.lineWidth = 1.0f;
    configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
    configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
    configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
    configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

    configInfo.multisampleInfo = {};
    configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    configInfo.colorBlendAttachment = {};
    configInfo.colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
    configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    configInfo.colorBlendInfo = {};
    configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
    configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    configInfo.colorBlendInfo.attachmentCount = 1;
    configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
    configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

    configInfo.depthStencilInfo = {};
    configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    configInfo.depthStencilInfo.depthTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.depthWriteEnable = VK_FALSE;
    configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;

    configInfo.viewportInfo = {};
    configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    configInfo.viewportInfo.viewportCount = 1;
    configInfo.viewportInfo.scissorCount = 1;
    configInfo.viewportInfo.pViewports = nullptr;
    configInfo.viewportInfo.pScissors = nullptr;

    configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    configInfo.dynamicStateInfo = {};
    configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
    configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();

    configInfo.renderPass = VK_NULL_HANDLE;
    configInfo.subpass = 0;

    return configInfo;
}

PipelineBuilder& PipelineBuilder::setShaders(const Shader& vert, const Shader& frag) {
    // Implementation for setting shaders

    m_config.vertShaderStageInfo = {};
    m_config.vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_config.vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    m_config.vertShaderStageInfo.module = vert.module;
    m_config.vertShaderStageInfo.pName = "main";

    m_config.fragShaderStageInfo = {};
    m_config.fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_config.fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    m_config.fragShaderStageInfo.module = frag.module;
    m_config.fragShaderStageInfo.pName = "main";

    return *this;
}

PipelineBuilder& PipelineBuilder::setVertexLayout(const VertexInputDescription& layout) {
    // Implementation for setting vertex layout

    m_config.vertexInputInfo = {};
    m_config.vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    m_config.vertexBindingDescriptions = layout.bindings;
    m_config.vertexAttributeDescriptions = layout.attributes;

    m_config.vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_config.vertexBindingDescriptions.size());
    m_config.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_config.vertexAttributeDescriptions.size());
    m_config.vertexInputInfo.pVertexBindingDescriptions = m_config.vertexBindingDescriptions.data();
    m_config.vertexInputInfo.pVertexAttributeDescriptions = m_config.vertexAttributeDescriptions.data();

    return *this;
}

PipelineBuilder& PipelineBuilder::setTopology(VkPrimitiveTopology topology) {
    // Implementation for setting topology

    m_config.inputAssemblyInfo = {};
    m_config.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    m_config.inputAssemblyInfo.topology = topology;
    m_config.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    return *this;
}

PipelineBuilder& PipelineBuilder::setDepthTest(bool enable, bool write) {
    m_config.depthStencilInfo = {};
    m_config.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    m_config.depthStencilInfo.depthTestEnable = enable ? VK_TRUE : VK_FALSE;
    m_config.depthStencilInfo.depthWriteEnable = write ? VK_TRUE : VK_FALSE;
    m_config.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    m_config.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    m_config.depthStencilInfo.stencilTestEnable = VK_FALSE;

    return *this;
}

PipelineBuilder& PipelineBuilder::setBlending(bool enable) {
    // Implementation for setting blending
    
    m_config.colorBlendAttachment = {};
    m_config.colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    m_config.colorBlendAttachment.blendEnable = enable ? VK_TRUE : VK_FALSE;

    m_config.colorBlendInfo = {};
    m_config.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    m_config.colorBlendInfo.logicOpEnable = VK_FALSE;
    m_config.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    m_config.colorBlendInfo.attachmentCount = 1;
    m_config.colorBlendInfo.pAttachments = &m_config.colorBlendAttachment;
    m_config.colorBlendInfo.blendConstants[0] = 0.0f;
    m_config.colorBlendInfo.blendConstants[1] = 0.0f;
    m_config.colorBlendInfo.blendConstants[2] = 0.0f;
    m_config.colorBlendInfo.blendConstants[3] = 0.0f;

    return *this;
}

PipelineBuilder& PipelineBuilder::setRenderPass(VkRenderPass renderPass, uint32_t subpass) {
    // Implementation for setting render pass

    m_config.renderPass = renderPass;
    m_config.subpass = subpass;

    return *this;
}

Pipeline PipelineBuilder::build() {

    Pipeline pipeline {};

    // TODO: create descriptor set layouts
    // TODO: create push constant ranges

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &pipeline.descriptorSetLayouts[0];

    if (vkCreatePipelineLayout(m_ctx.device, &pipelineLayoutInfo, nullptr, &pipeline.layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    VkPipelineShaderStageCreateInfo shaderStages[2] = {
        m_config.vertShaderStageInfo,
        m_config.fragShaderStageInfo
    };
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &m_config.vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &m_config.inputAssemblyInfo;
    pipelineInfo.pViewportState = &m_config.viewportInfo;
    pipelineInfo.pRasterizationState = &m_config.rasterizationInfo;
    pipelineInfo.pMultisampleState = &m_config.multisampleInfo;
    pipelineInfo.pDepthStencilState = &m_config.depthStencilInfo;
    pipelineInfo.pColorBlendState = &m_config.colorBlendInfo;
    pipelineInfo.pDynamicState = &m_config.dynamicStateInfo;
    pipelineInfo.layout = pipeline.layout;
    pipelineInfo.renderPass = m_config.renderPass;
    pipelineInfo.subpass = m_config.subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(m_ctx.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    return pipeline;
}

}; // namespace renderer::utils