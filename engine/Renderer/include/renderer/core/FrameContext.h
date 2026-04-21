#pragma once

#include <core/types.h>

#include <vulkan/vulkan.h>

// represent a frame in flight
struct FrameContext {
    VkCommandBuffer cmd; // primary command buffer for the frame

    // Synchronization primitives for the frame
    VkFence fence;
    VkSemaphore imageAvailable;
    VkSemaphore renderFinished;

    u32 imageIndex; // acquired image index for the frame
    u32 frameIndex; // current frame index (0 to MAX_FRAMES_IN_FLIGHT-1)

    VkExtent2D extent; // current swapchain extent for the frame
};