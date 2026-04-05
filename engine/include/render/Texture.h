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
    Sampled  = 1 << 0,
    Storage  = 1 << 1,
    Render   = 1 << 2,
    CopySrc  = 1 << 3,
    CopyDst  = 1 << 4
};

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

struct Texture {
    VkImage image = {VK_NULL_HANDLE};
    VkImageView imageView = {VK_NULL_HANDLE};
    VkSampler sampler = {VK_NULL_HANDLE};

    int width = 0;
    int height = 0;
    int numChannels = 0;
    
    VkFormat format = VK_FORMAT_UNDEFINED;
    u32 mipmaps;

    // Debug
    std::string debugName;
};