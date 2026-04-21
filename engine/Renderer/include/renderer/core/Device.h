#pragma once

#include <core/types.h>
#include <core/Log.h>

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <set>
#include <optional>

#include "Instance.h"

namespace renderer::core {

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

// TODO: clean up functions

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
    bool initialize(
        VkInstance instance, 
        VkSurfaceKHR surface, 
        std::vector<const char*> deviceExtensions,
        bool enableValidationLayers,
        std::vector<const char*> validationLayers
    );
    void cleanup();

    SwapChainSupportDetails getSwapChainSupport() {
        return querySwapChainSupport(m_physicalDevice);
    }

    QueueFamilyIndices findQueueFamilies() {
        return findQueueFamilies(m_physicalDevice);
    }

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

    // Initialization
    bool pickPhysicalDevice();
    bool createLogicalDevice();

    void createCommandPool();
    //void allocateCommandBuffer(VkCommandBuffer *buffers, uint32_t size);

private:
    VkInstance m_instance;
    VkSurfaceKHR m_surface;
    std::vector<const char*> m_deviceExtensions;
    bool m_enableValidationLayers = false;
    std::vector<const char*> m_validationLayers;


    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device;

    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    VkCommandPool m_commandPool;

};

}; // namespace renderer::core