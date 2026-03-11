#pragma once

#include "defines.h"
#include "Instance.h"

// GPU objs
#include "Buffer.h"

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
    bool createBuffer(const BufferDescriptor& desc, Buffer& buffer);
    void uploadData(Buffer&, const void* data, VkDeviceSize size);
    void releaseBuffer(Buffer& buffer);

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