#pragma once

#include <core/types.h>
#include <core/Log.h>

#include <string>
#include <memory>

#include <vulkan/vulkan.h>

#include <platform/ISurface.h>

#include <renderer/core/Context.h>

#include <renderer/core/Instance.h>
#include <renderer/core/Device.h>
#include <renderer/core/SwapChain.h>

namespace renderer {

struct GraphicsDeviceDesc {
    bool        enableValidation  = true;   // activa validation layers
    uint32_t    framesInFlight    = 2;
    std::string appName           = "App";

    std::vector<const char*> extensions;
};

enum class DeviceState : u32 {
    Uninitialized,
    Ready,
    Destroyed,
    LostSurface,
    Invalid
};

// State: Uninitialize, Ready, Destroyed, Invalid, LostSurface, etc
class GraphicsDevice {
public:
    GraphicsDevice() = default;

    // No copyable, no movable (own Vulkan handles)
    GraphicsDevice(const GraphicsDevice&) = delete;
    GraphicsDevice& operator=(const GraphicsDevice&) = delete;

    bool create(platform::ISurface&, const GraphicsDeviceDesc&);
    void destroy();
    
    //Context getContext() const;

    // Swapchain access
    core::SwapChain&       swapchain();
    const core::SwapChain& swapchain() const;

    core::Context getContext() const;

    bool isValid() const { return true; /* TODO */ };

    VkExtent2D extent()      const { return m_swapchain->getExtent(); }
    VkFormat   colorFormat() const { return m_swapchain->getImageFormat(); }
    uint32_t   imageCount()  const { return m_swapchain->imageCount(); }

    void recreateSwapChain();

public:
    // Event handlers
    void onResized(uint32_t width, uint32_t height);

private:
    core::Instance m_instance;
    core::Device m_device;

    u32 m_framesInFlight;

    std::unique_ptr<core::SwapChain> m_swapchain;

    VkSurfaceKHR     m_surface     = VK_NULL_HANDLE;

    //Context m_context;
    
    //VkPhysicalDevice m_gpu         = VK_NULL_HANDLE;
    //VkDevice         m_device      = VK_NULL_HANDLE;
    //VkCommandPool    m_commandPool = VK_NULL_HANDLE;
    //VkQueue          m_graphicsQueue;
    //VkQueue          m_presentQueue;
    
};

};