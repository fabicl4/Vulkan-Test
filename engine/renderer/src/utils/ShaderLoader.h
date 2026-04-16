#pragma once

#include <core/types.h>

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

#include "resources/Shader.h"

#include "vulkan/Context.h"

class ShaderLoader {
public:
    explicit ShaderLoader(Device& device) : m_device(device) {}

    Shader load(const std::string& vsPath, VkShaderStageFlagBits stage);

private:
    // Helper functions for shader loading and Vulkan resource creation.
    std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    Device& m_device;
};