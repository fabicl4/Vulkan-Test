#pragma once

#include "SubRenderer.h"

class TestRenderer : public SubRenderer 
{
public:
    virtual ~TestRenderer() override = default;

    // Initialize SubRenderer data
    virtual bool initialize(VkRenderPass renderPass) override {
        createPipeline(m_device, renderPass);

        m_device.createVertexBuffer(vertices, m_vertexBuffer);
        m_device.createIndexBuffer(indices, m_indexBuffer);

        return true;
    };

    // call by the renderer
    void recordCommands(VkCommandBuffer commandBuffer, uint32_t imageIndex) override {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.handle);

        VkBuffer vertexBuffers[] = {m_vertexBuffer.handle};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer.handle, 0, VK_INDEX_TYPE_UINT16);

        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    }

    // release SubRenderer data 
    void cleanup() override 
    {
        m_device.destroyBuffer(m_vertexBuffer);
        m_device.destroyBuffer(m_indexBuffer);
        
        m_device.destroyPipeline(m_pipeline);
    };

private:
    void createPipeline(Device& device, VkRenderPass renderPass);

private:
    Pipeline m_pipeline;
    
    const std::vector<float> vertices = {
    //  POSITION       ||   COLOR
        -0.5f, -0.5f,       1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,       0.0f, 1.0f, 0.0f,
         0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f,        1.0f, 1.0f, 1.0f
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    // Buffers
    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;
};