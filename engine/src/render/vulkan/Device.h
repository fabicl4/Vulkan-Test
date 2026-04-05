#pragma once

#include "defines.h"
#include "Instance.h"

#include <render/vulkan/Buffer.h>
#include "Pipeline.h"

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

/* 
 *  Represents the GPU and manage the GPU's resources.
 *  Is responsable of:
 *  - Choose the physical device
 *  - Check capabilities
 *  - Create the logical device.
 *  - Create queues
 *  - Create CommandPool
 *  - Helper functions to create GPU resources
 * 
 *  Dependencies:
 *  - Instance
 *  - Surface
*/
class Device {
public:
    Device(VkInstance instance, VkSurfaceKHR surface)
        : m_instance(instance), m_surface(surface) {}

    bool initialize() {
        if (!pickPhysicalDevice())
            return false;

        if (!createLogicalDevice())
            return false;

        createCommandPool();

        return true;
    }

    void cleanup() {
        vkDestroyDevice(m_device, nullptr);
    }

    SwapChainSupportDetails getSwapChainSupport() {
        return querySwapChainSupport(m_physicalDevice);
    }

    QueueFamilyIndices findQueueFamilies() {
        return findQueueFamilies(m_physicalDevice);
    }

    // Envía un command buffer con sincronización básica.
    // Espera 'waitSemaphore' antes de empezar, señaliza 'signalSemaphore' al terminar,
    // y activa 'fence' cuando la ejecución complete.
    VkResult submitCommandBuffer(
        VkCommandBuffer cmd,
        VkSemaphore waitSemaphore,
        VkSemaphore signalSemaphore,
        VkFence fence
    ) {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {waitSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        VkCommandBuffer commandBuffers[] = {cmd};
        submitInfo.pCommandBuffers = commandBuffers;

        VkSemaphore signalSemaphores[] = {signalSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        return vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, fence);
    }


public:
    // Buffer
    //-------------------------------------------------------------------------
    bool createVertexBuffer(const std::vector<float>& vertices, Buffer& vertexBuffer);
    bool createIndexBuffer(const std::vector<uint16_t>& indices, Buffer& indexBuffer);
    
    void destroyBuffer(Buffer& buffer);
    
private:
    bool createBuffer(const BufferDescriptor& desc, Buffer& buffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    //void uploadData(Buffer&, const void* data, VkDeviceSize size);
    
    // Helpers

    // Image
    //-------------------------------------------------------------------------
    bool createTextureImage(int width, int height, int numChannels, unsigned char* data, 
        VkImage& image, VkDeviceMemory& imageMemory);

private:
    // Helpers 
    void createImage(uint32_t width, uint32_t height, 
        VkFormat format, 
        VkImageTiling tiling, 
        VkImageUsageFlags usage, 
        VkMemoryPropertyFlags properties, 
        VkImage& image, VkDeviceMemory& imageMemory);

    

public:    
    // Shader
    //-------------------------------------------------------------------------
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void destroyShaderModule(VkShaderModule shaderModule);

    // Pipeline
    //-------------------------------------------------------------------------
    //Pipeline createGraphicsPipeline(
    //    const std::vector<char>& vsCode, const std::vector<char>& fsCode, const PipelineConfigInfo& configInfo);

    void bindPipeline(VkCommandBuffer commandBuffer,
        const Pipeline& pipeline) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.handle);
    }

    void createGraphicsPipeline(VkShaderModule vertShader, VkShaderModule fragShader, 
        const PipelineConfigInfo& pipelineInfo, Pipeline& pipeline);

    void destroyPipeline(Pipeline& pipeline);
public:

    //-------------------------------------------------------------------------
    // Get native vulkan objects
    VkDevice getDevice() const { return m_device; }
    VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
    VkQueue getGraphicsQueue() const { return m_graphicsQueue; }
    VkQueue getPresentQueue() const { return m_presentQueue; }
    VkCommandPool getCommandPool() const {return m_commandPool;}

private:
    // select device
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    // helpers
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    //TODO:
    //void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
private:
    // Initialization
    bool pickPhysicalDevice();
    bool createLogicalDevice();

    void createCommandPool();
    //void allocateCommandBuffer(VkCommandBuffer *buffers, uint32_t size);

private:
    VkInstance m_instance;
    VkSurfaceKHR m_surface;

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device;

    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    VkCommandPool m_commandPool;

};