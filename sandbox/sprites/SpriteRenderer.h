#pragma once

#include "SubRenderer.h"

struct SpriteData {
    unsigned int spritesheet_id;
    glm::vec2 position;
    glm::vec2 scale;
    float rotation;
    glm::vec4 color;
};

struct SpriteSheet {
    VkImage image;
    VkImageView imageView;
    glm::ivec2 image_size;

    glm::ivec2 margins;
    glm::ivec2 offset;
    
    glm::ivec2 spriteSize;
};

class SpriteRenderer : public SubRenderer {
public:
    SpriteRenderer(const std::string& path) {}

    virtual ~SpriteRenderer() = default;
    
    virtual bool initialize(Device& device, VkRenderPass renderPass) override {
        createVertexBuffer(device);
        createIndexBuffer(device);
        createPipeline(device, renderPass);
        createDescriptors(device);
        createCameraBuffer(device);
        createSpriteSheetBuffer(device);
        createSpriteInstanceBuffer(device);
    }

    virtual void recordCommands(VkCommandBuffer cmd, uint32_t imageIndex) override 
    {
        updateBuffers();
        

    }

    virtual void cleanup(Device& device) override
    {

    }

    void onBeginFrame() {
        m_sprites.clear();
        m_instances.clear();
    }

    void onEndFrame() {}

    void submit(SpriteData data) {
        if (num_instances + 1 >= MAX_INSTANCES) {
            // buffer is full!
            return;
        }

        m_sprites.push_back(data);
    }

private:
    void createVertexBuffer(Device& device);
    void createIndexBuffer(Device& device);
    void createPipeline(Device& device, VkRenderPass renderPass);
    void createDescriptors(Device& device);
    void createCameraBuffer(Device& device);
    void createSpriteSheetBuffer(Device& device);
    void createSpriteInstanceBuffer(Device& device);

    void updateBuffers() {

    }

    VkPipeline m_pipeline;
    VkPipelineLayout m_pipelineLayout;

    const std::vector<float> vertices = {
        // Position   |     UVs
        -0.5f, -0.5f,   1.0f, 0.0f,
         0.5f, -0.5f,   0.0f, 1.0f,
         0.5f,  0.5f,   0.0f, 0.0f,
        -0.5f,  0.5f,   1.0f, 1.0f
    };
    
    const int numVertices = 4;

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    const int numIndices = 6;

    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;

    VkDescriptorPool m_descriptorPool;

    std::array<VkDescriptorSet, 2> m_cameraSet;        // set 0
    VkDescriptorSet m_spriteSheetSet;   // set 1
    std::array<VkDescriptorSet, 2> m_instanceSet;      // set 2
    VkDescriptorSet m_textureSet;       // set 3
    
    VkDescriptorSetLayout m_cameraSetLayout;        // set 0
    VkDescriptorSetLayout m_spriteSheetSetLayout;   // set 1
    VkDescriptorSetLayout m_instanceSetLayout;      // set 2
    VkDescriptorSetLayout m_textureSetLayout;       // set 3

    std::vector<VkDescriptorSet> descriptorSets;

    std::array<Buffer, 2> m_cameraBuffers;
    Buffer m_spriteSheetBuffer;
    std::array<Buffer, 2> m_spriteInstanceBuffer;

    // load from file
    bool loadSpriteSheet(const std::string& path, 
        const glm::ivec2& margins, 
        const glm::ivec2& offset, 
        const glm::ivec2& spriteSize);

    SpriteSheet m_spriteSheet;

    // Frame data
    //-------------------------------------------------------------------------
    std::vector<SpriteData> m_sprites {};

    struct alignas(16) SpriteInstance { // process data submitted to the GPU
        glm::mat4 model;
        glm::vec4 color;
    };
    std::vector<SpriteData> m_instances {};

    const int MAX_INSTANCES = 100;
    // buffer max size!
    const int MAX_INSTANCES_SIZE = MAX_INSTANCES * sizeof(SpriteInstance);

    // current number of instances!
    int num_instances{0};
};