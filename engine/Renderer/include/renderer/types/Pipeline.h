#pragma once

#include <core/types.h>
#include <core/Log.h>

#include <vulkan/vulkan.h>

/*
 *  The Graphics Pipeline
 *  ----------------------
 * 
 *  Input: Vertex/Index Buffer
 *   
 *  STAGE 1: Input Assembler
 *  STAGE 2: Vertex Shader
 *  STAGE 3: Rasterization
 *  STAGE 4: Fragment Shader
 *  STAGE 5: Color Blending
 *  
 *  Output: FrameBuffer (image)
 */

// There are 2 types of pipeline in Vulkan: Graphics and Compute pipelines

// Graphics pipeline object

namespace renderer {

struct Pipeline {
    VkPipeline       handle = VK_NULL_HANDLE;
    VkPipelineLayout layout = VK_NULL_HANDLE;

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

    bool isValid() const { return handle != VK_NULL_HANDLE; }

    void destroy(VkDevice device) {
        if (handle) vkDestroyPipeline(device, handle, nullptr);
        if (layout) vkDestroyPipelineLayout(device, layout, nullptr);
        for (auto dsl : descriptorSetLayouts)
            vkDestroyDescriptorSetLayout(device, dsl, nullptr);
        *this = {};
    }

    // Helpers para el command buffer — el pass los llama en doRecordCommands
    void bind(VkCommandBuffer cmd) const {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, handle);
    }
    void bindDescriptorSet(VkCommandBuffer cmd, uint32_t set,
                           VkDescriptorSet ds) const {
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, set, 1, &ds, 0, nullptr);
    }
};

}; // namespace renderer::resource