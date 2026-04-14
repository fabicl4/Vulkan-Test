#pragma once

#include <vulkan/vulkan.h>

#include <vector>

struct RenderTarget {
    std::vector<VkFramebuffer> framebuffers;
    VkExtent2D extent;
    VkFormat colorFormat = VK_FORMAT_UNDEFINED;
    // opcional: VkImageView colorViews; depthViews; etc.
};