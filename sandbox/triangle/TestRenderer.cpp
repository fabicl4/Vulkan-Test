#include "TestRenderer.h"

void TestRenderer::createPipeline(Device& device, VkRenderPass renderPass){

    const Shader* shader = m_resourceSystem.get<Shader>("test");

    VkShaderModule vertShaderModule = shader->vsModule;
    VkShaderModule fragShaderModule = shader->fsModule;

    PipelineConfigInfo configInfo = defaultPipelineConfigInfo(800, 600); // TODO: CHANGE THIS LINE!

    // Define vertex layout
    VertexLayout layout ({
        {"Position", 0, VertexAttributeFormat::FLOAT2},
        {"Color", 1, VertexAttributeFormat::FLOAT3},
    });

    
    configInfo.setVertexLayout(layout);
    configInfo.renderPass = renderPass;
    

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, 
        &m_pipeline.layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    configInfo.pipelineLayout = m_pipeline.layout;

    m_device.createGraphicsPipeline(vertShaderModule, fragShaderModule, configInfo, 
        m_pipeline);
}