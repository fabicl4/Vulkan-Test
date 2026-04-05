#pragma once

#include <vulkan/vulkan.h>

struct RenderTarget {
    VkImage image;
    VkImageView view;
    VkFormat format;
};