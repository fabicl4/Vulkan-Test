#pragma once

#include <vulkan/vulkan.h>

struct Context {
    Context(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue) :
        device(device), physicalDevice(physicalDevice), commandPool(commandPool), graphicsQueue(graphicsQueue) {}

    VkDevice         device;
    VkPhysicalDevice physicalDevice;
    VkCommandPool    commandPool;
    VkQueue          graphicsQueue;

    //BufferAllocator* bufferAllocator = nullptr;

    bool isValid() const { return device != VK_NULL_HANDLE; }

    // Helper functions for resource creation. 
    // These are just convenient wrappers around Vulkan calls, 
    // but they can be extended to include more complex logic 
    // (like staging buffers, memory pooling, etc.)
    void createBuffer(VkDeviceSize, VkBufferUsageFlags,
                      VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory&);
    void createImage(uint32_t w, uint32_t h, VkFormat, VkImageTiling,
                     VkImageUsageFlags, VkMemoryPropertyFlags,
                     VkImage&, VkDeviceMemory&);

    VkShaderModule createShaderModule(const std::vector<char>& code);

    VkCommandBuffer beginSingleTimeCommands();
    void            endSingleTimeCommands(VkCommandBuffer);

    uint32_t findMemoryType(uint32_t filter, VkMemoryPropertyFlags) const;
};