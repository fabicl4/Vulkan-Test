#pragma once

#include <renderer/core/Context.h> 

#include <renderer/types/Buffer.h>

namespace renderer {

class GraphicsDevice;

class BufferAllocator {
public:
    BufferAllocator(core::Context ctx) : m_ctx(ctx) {};

    bool createBuffer(const BufferDescriptor& desc, Buffer& buffer);

private:
    core::Context m_ctx;
    // TODO: VMA Allocator

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

};

}; // namespace renderer