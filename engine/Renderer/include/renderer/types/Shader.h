#pragma once

#include <core/types.h>

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

/*
struct Shader {
    VkShaderModule vsModule = VK_NULL_HANDLE;
    VkShaderModule fsModule = VK_NULL_HANDLE;

    // Debug
    std::string debugName;

    std::string vsPath;
    std::string fsPath;
};
*/

namespace renderer {

struct Shader {
    VkShaderModule       module = VK_NULL_HANDLE;
    VkShaderStageFlagBits stage  = VK_SHADER_STAGE_VERTEX_BIT;
    std::string          entryPoint = "main"; // casi siempre "main"

    // Debug
    std::string debugName;
    std::string filepath;

    bool isValid() const { return module != VK_NULL_HANDLE; }
    void destroy(VkDevice device) {
        if (module != VK_NULL_HANDLE) {
            vkDestroyShaderModule(device, module, nullptr);
            module = VK_NULL_HANDLE;
        }
    }
};

}; // namespace renderer::resource