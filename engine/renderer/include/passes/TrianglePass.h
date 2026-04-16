#pragma once 

#include <render/pipeline/RenderPass.h>

// Remove this include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};


// TODO: This pass is just for testing. It should be removed or refactored later.
class TrianglePass : public RenderPass {
public:
    TrianglePass(Context ctx) : RenderPass(ctx) {}

    // Lifecycle
    bool initialize(RenderTarget& renderTarget) override;

    void prepare() override;
    // Cleanup resources created by the render pass.
    void cleanup() override;
    
private:
    void doRecordCommands(
        VkCommandBuffer commandBuffer, 
        uint32_t imageIndex,
        uint32_t frameIndex, 
        VkFramebuffer framebuffer, 
        const VkExtent2D& extent
    ) override;

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

    VkDescriptorSetLayout m_descriptorSetLayout;

    void createDescriptorSetLayout();

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    void createUniformBuffers();
    void updateUniformBuffer(uint32_t currentImage, VkExtent2D extent);

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    
    void createDescriptorPool();
    void createDescriptorSets();

    Shader m_vertShader;
    Shader m_fragShader;
    Pipeline m_pipeline;
    
};