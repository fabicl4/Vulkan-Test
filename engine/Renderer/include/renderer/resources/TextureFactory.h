#pragma once

#include <vulkan/vulkan.h>

#include <core/types.h>
#include <core/Log.h>

#include <renderer/core/Context.h>

#include <renderer/types/Texture.h>

namespace renderer {

class TextureFactory {
public:
    TextureFactory(core::Context ctx) : m_ctx(ctx) {}

    // load texture from file and create a Vulkan texture object.
    Texture loadTextureFromFile(const std::string& filepath);
    Texture createAttachment(const TextureDescriptor& desc);

private:

    void createImage(uint32_t width, uint32_t height, 
        VkFormat format, 
        VkImageTiling tiling, 
        VkImageUsageFlags usage, 
        VkMemoryPropertyFlags properties, 
        VkImage& image, VkDeviceMemory& imageMemory
    );

    // Helper functions for texture loading and Vulkan resource creation.
    // These can be extended to include more complex logic (like staging buffers, mipmap generation
    // etc.) as needed.
    void transitionLayout(VkImage image, VkFormat format,
                          VkImageLayout oldLayout, VkImageLayout newLayout,
                          uint32_t mipLevels = 1);
    void copyBufferToImage(VkBuffer buffer, VkImage image,
                           uint32_t width, uint32_t height);

    VkImageView  createView(VkImage image, VkFormat format,
                            VkImageAspectFlags aspect, uint32_t mipLevels);
    VkSampler    createSampler(uint32_t mipLevels);

    core::Context m_ctx;
};

}; // namespace renderer