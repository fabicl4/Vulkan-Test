#pragma once

#include <render/SubRenderer.h>

class TestRenderer : public SubRenderer {
public:
    bool initialize(Device* device, VkRenderPass renderPass) override;
    void recordCommands(VkCommandBuffer cmd, uint32_t imageIndex) override;
    void cleanup(Device* device) override;

private:
    void createVertexBuffer(Device* device);
    void createIndexBuffer(Device* device);

private:
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

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
};