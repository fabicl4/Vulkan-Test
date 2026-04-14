#pragma once 

#include <render/RenderPass.h>

#include "../vulkan/Vertex.h"

class TrianglePass : public RenderPass {
public:
    TrianglePass(Device& device, const Config& config = {} ) : RenderPass(device, config) {}

    // Lifecycle
    bool initialize(RenderTarget& renderTarget) override;
    // Called when the render target is resized. The render pass can choose to
    // recreate framebuffers or other resources.
    void prepare() override;
    // Cleanup resources created by the render pass.
    void cleanup() override;
    // Called when the render target is resized.
    //void resize(RenderTarget& renderTarget) override {};
    
private:
    void doRecordCommands(VkCommandBuffer commandBuffer, uint32_t imageIndex, 
        VkFramebuffer framebuffer, const VkExtent2D& extent) override;

private:
    void createVertexBuffer();
    void createIndexBuffer();

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

    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_pipeline;

    void createPipeline();
};