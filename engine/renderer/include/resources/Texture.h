#pragma once

#include <core/types.h>

#include <vulkan/vulkan.h>
#include <string>

enum class TextureFormat {
    RGBA8,
    BGRA8,
    RGBA16F,
    Depth24Stencil8
};

enum class TextureUsage : u32 {
    Sampled         = 1 << 0,
    ColorAttachment = 1 << 1,  // render target color
    DepthAttachment = 1 << 2,  // render target depth
    TransferDst     = 1 << 3,
    TransferSrc     = 1 << 4,
    Storage         = 1 << 5,
};

inline TextureUsage operator|(TextureUsage a, TextureUsage b) {
    return static_cast<TextureUsage>(
        static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

enum class TextureType : u32
{
    TEXTURE_1D,
    TEXTURE_2D,
    TEXTURE_3D
};

enum TextureFilterMode : u32
{
    LINEAR,
    NEAREST
};

enum TextureWrapMode : u32
{
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER
};

enum class PixelFormat : u32
{
    None = 0,
    R8,
    RGB8,
    RGBA8,
    RGBA32F
};

struct TextureDescriptor {
    uint32_t     width;
    uint32_t     height;
    VkFormat     format      = VK_FORMAT_R8G8B8A8_SRGB;
    TextureUsage usage       = TextureUsage::Sampled | TextureUsage::TransferDst;
    uint32_t     mipLevels   = 1;
    bool         createView    = true;
    bool         createSampler = true;  // false para depth attachments o uso compute
};

struct Texture {
    VkImage image = {VK_NULL_HANDLE};
    VkDeviceMemory memory  = VK_NULL_HANDLE;
    VkImageView imageView = {VK_NULL_HANDLE};
    VkSampler sampler = {VK_NULL_HANDLE};
    
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkExtent2D extent     = {};
    u32 mipmaps = 1;
    
    // Debug
    std::string debugName;
    int width = 0;
    int height = 0;
    int numChannels = 0;

    bool isValid() const { return image != VK_NULL_HANDLE; }

    void destroy(Context& ctx) {
        if (sampler) vkDestroySampler(ctx.device, sampler, nullptr);
        if (imageView)    vkDestroyImageView(ctx.device, imageView, nullptr);
        if (image)   vkDestroyImage(ctx.device, image, nullptr);
        if (memory)  vkFreeMemory(ctx.device, memory, nullptr);
        *this = {};
    }
};