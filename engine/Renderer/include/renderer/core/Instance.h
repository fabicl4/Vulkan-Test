#pragma once

#include <vulkan/vulkan.h>

#include <core/types.h>
#include <core/Log.h>

/*
Represent global Vulkan Instance.
- Create the Vulkan instance
- Activate (if enabled) the validation layers
- Activate the global extensions
- Destroy the instance
*/

namespace renderer::core {

class Instance {
public:
    bool initialize(
        bool enableValidationLayers,
        std::vector<const char*> extensions,
        std::vector<const char*> validationLayers
    );

    void cleanup();

    VkInstance handle() const { return m_instance; }

private:
    void createInstance();
    void setupDebugMessenger();

    // helpers
    bool checkValidationLayerSupport();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    //void destroyDebugMessenger();

private:
    VkInstance m_instance{VK_NULL_HANDLE};

    VkDebugUtilsMessengerEXT m_debugMessenger{VK_NULL_HANDLE};

    bool m_enableValidationLayers{false};

    std::vector<const char*> m_extensions {};
    std::vector<const char*> m_validationLayers {};
};

}; // namespace renderer::core 