#pragma once

#include <core/types.h>

#include <vulkan/vulkan.h>
#include <string>

struct Shader {
    VkShaderModule vsModule = VK_NULL_HANDLE;
    VkShaderModule fsModule = VK_NULL_HANDLE;

    // Debug
    std::string debugName;

    std::string vsPath;
    std::string fsPath;
};