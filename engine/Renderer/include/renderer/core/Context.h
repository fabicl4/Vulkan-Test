#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>

namespace renderer::core {

struct Context {
    VkDevice         device         = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkCommandPool    commandPool    = VK_NULL_HANDLE;
    VkQueue          graphicsQueue  = VK_NULL_HANDLE;
    uint32_t         framesInFlight = 2;

    bool isValid() const { return device != VK_NULL_HANDLE; }
};

} // namespace renderer::core