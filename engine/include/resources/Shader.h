#pragma once

#include <string>
#include <vector>

#include "Resource.h"

#include <core/Log.h>
#include <core/types.h>

#include <render/Shader.h>

class Device;

using ShaderHandle = Handle<Shader>;

class ShaderManager : public ResourceManagerBase<Shader> {
public:
    explicit ShaderManager(Device& device) : m_device(device) {}
    
    bool load(const std::string& name, 
        const std::string& vsPath, const std::string& fsPath);

    virtual void release(const std::string& name) override;

private:
    std::vector<char> readFile(const std::string& filename);

    // Factory method to create shader modules
    VkShaderModule createShaderModule(const std::vector<char>& code);
    
    VkResult createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

    Device& m_device;
};