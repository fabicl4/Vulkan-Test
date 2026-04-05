#include <resources/Shader.h>

#include <iostream>
#include <fstream>
#include <exception>
#include <stdexcept>

#include <render/vulkan/Device.h>

std::vector<char> ShaderManager::readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkResult ShaderManager::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
    VkShaderModuleCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    return vkCreateShaderModule(m_device.getDevice(), &createInfo, nullptr, shaderModule);
}

bool ShaderManager::load(const std::string& name, 
    const std::string& vsPath, const std::string& fsPath) {
    try {
        auto vertShaderCode = readFile(vsPath);
        auto fragShaderCode = readFile(fsPath);

        VkShaderModule vertShaderModule = m_device.createShaderModule(vertShaderCode);
        if (vertShaderModule == VK_NULL_HANDLE) {
            LOG_ERROR("Could not create the vertex shader module!");
            return false;
        }
        
        VkShaderModule fragShaderModule = m_device.createShaderModule(fragShaderCode);
        if (vertShaderModule == VK_NULL_HANDLE) {
            LOG_ERROR("Could not create the fragment shader module!");
            return false;
        }

        Shader shader{};
        shader.vsModule = vertShaderModule;
        shader.fsModule = fragShaderModule;
        shader.debugName = name;
        shader.vsPath = vsPath;
        shader.fsPath = fsPath;

        m_cache.emplace(name, shader);

    } catch (const std::exception& e) {
        LOG_CRITICAL("Exception caught: {}", e.what());
        return false;
    }

    return true;
}

void ShaderManager::release(const std::string& name) {
    auto it = m_cache.find(name);
    if (it == m_cache.end()) { // The resource does NOT exist! 
        return;
    }

    Shader shader = it->second;

    LOG_INFO("Release shader {}!", shader.debugName);

    m_device.destroyShaderModule(shader.vsModule);
    m_device.destroyShaderModule(shader.fsModule);

    m_cache.erase(it);
}
