#pragma once

#include <core/types.h>
#include <core/Log.h>

// PLATFORM LAYER (VULKAN)
#include <render/vulkan/Device.h>

#include "vulkan/Buffer.h"
#include "vulkan/Vertex.h"
//#include "vulkan/Pipeline.h"

// RESOURCE LAYER
#include <resources/Resource.h>
#include <resources/Shader.h>
//#include <resources/Texture.h>

class SubRenderer {
public:
    SubRenderer(Device& device, ResourceSystem& resourceSystem) :
        m_device(device), m_resourceSystem(resourceSystem) {}

    virtual ~SubRenderer() = default;
    
    virtual bool initialize(VkRenderPass renderPass) = 0;
    virtual void cleanup() = 0;
    
    virtual void recordCommands(VkCommandBuffer cmd, uint32_t imageIndex) = 0;

protected:
    Device& m_device;
    ResourceSystem m_resourceSystem;
};
