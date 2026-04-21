#pragma once

#include <core/types.h>
#include <core/Log.h>

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

#include <renderer/core/Context.h>

#include <renderer/types/Shader.h>

namespace renderer {

class GraphicsDevice;

class ShaderLoader {
public:
    explicit ShaderLoader(core::Context ctx) : m_ctx(ctx) {}

    Shader load(const std::string& vsPath, VkShaderStageFlagBits stage);

private:
    // Helper functions for shader loading and Vulkan resource creation.
    std::vector<char> readFile(const std::string& filename);
    
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void destroyShaderModule(VkShaderModule shaderModule);

    core::Context m_ctx;
};

}; // namespace renderer