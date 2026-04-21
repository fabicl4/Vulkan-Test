#pragma once

//#include "RenderPass.h"

#include <vulkan/vulkan.h>

#include <core/types.h>
#include <core/Log.h>

namespace renderer {

// It handles:
//  - its pipeline
//  - descriptors
//  - buffers and instances
// implement method submit(...)

class SubRenderer {
public:
    virtual ~SubRenderer() = default;
    
    virtual bool initialize(Device* device, VkRenderPass renderPass) = 0;
    virtual void recordCommands(VkCommandBuffer cmd, uint32_t imageIndex) = 0;
    virtual void cleanup(Device* device) = 0;
};

}; // renderer::pipeline