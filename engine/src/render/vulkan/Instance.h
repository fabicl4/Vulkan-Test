#pragma once

#include "vkCore.h"

/*
Represent global Vulkan Instance.
- Create the Vulkan instance
- Activate (if enabled) the validation layers
- Activate the global extensions
- Destroy the instance
*/

class Instance {
public:
    Instance(bool enableValidationLayers = false) : 
        m_enableValidationLayers(enableValidationLayers) {};

    bool initialize() {
        createInstance();
        setupDebugMessenger();

        return true;
    }

    void cleanup() {
        if (m_enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
        }

        vkDestroyInstance(m_instance, nullptr);
    }

    VkInstance handle() const { return m_instance; }

private:
    void createInstance();
    void setupDebugMessenger();

    // helpers
    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    //void destroyDebugMessenger();

private:
    VkInstance m_instance{VK_NULL_HANDLE};

    VkDebugUtilsMessengerEXT m_debugMessenger{VK_NULL_HANDLE};

    bool m_enableValidationLayers{false};
};