#include <renderer/utils/ShaderLoader.h>

namespace renderer {

Shader ShaderLoader::load(const std::string& filepath, VkShaderStageFlagBits stage) {
    auto code = readFile(filepath);
};

std::vector<char> ShaderLoader::readFile(const std::string& filename) {
    // TODO: ...
    return {};
}

VkShaderModule ShaderLoader::createShaderModule(const std::vector<char>& code) {
    VkShaderModule shaderModule;

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }
    
    return shaderModule;
}

void ShaderLoader::destroyShaderModule(VkShaderModule shaderModule) {
    if (shaderModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(m_device, shaderModule, nullptr);
    }
}

};