#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include <renderer/types/Texture.h>

namespace renderer {

struct RenderTarget {
    std::vector<Texture> colorAttachments;

    // optional: Depth attachment 
    Texture depthAttachment;

    VkExtent2D extent {};

    bool isSwapChainTarget = false;

    bool isSwapchain() const { return isSwapChainTarget; }

    // Helpers
    bool hasDepth() const { return depthAttachment.isValid(); }
    uint32_t colorCount() const { return static_cast<uint32_t>(colorAttachments.size()); }
};

};