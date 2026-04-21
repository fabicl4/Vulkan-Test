#include <renderer/GraphicsDevice.h>

#include <renderer/core/Instance.h>
#include <renderer/core/Device.h>

#include <renderer/core/SwapChain.h>

namespace renderer {

bool GraphicsDevice::create(platform::ISurface& surface, const GraphicsDeviceDesc& desc) {

    m_framesInFlight = desc.framesInFlight;
    
    const std::vector<const char*> validationLayers   = { "VK_LAYER_KHRONOS_validation" };
    bool enableValidation;
    #ifdef NDEBUG
    enableValidation = false;
    #else
    enableValidation = true;
    #endif

    LOG_INFO("Creating Vulkan context...");
    LOG_INFO("Validation layers: {}", desc.enableValidation ? "enabled" : "disabled");

    // 1. Create VkInstance
    if (m_instance.initialize(desc.enableValidation, desc.extensions, validationLayers)) {
        return false;
    }

    // 2. Create surface
    m_surface = surface.createVulkanSurface(m_instance.handle());

    const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    // 3. Create: VkPhysicalDevice, VkDevice, VkQueue, VkCommandPool
    if (m_device.initialize(m_instance.handle(), m_surface, deviceExtensions, enableValidation, validationLayers) ) {
        return false;
    }

    int width;
    int height;
    surface.getFramebufferSize(width, height);

    // 4 Create the SwapChain
    VkExtent2D extent = {};
    extent.width = static_cast<u32>(width);
    extent.height = static_cast<u32>(height);

    m_swapchain = std::make_unique<core::SwapChain>( // TODO: Change this!
        m_device, 
        m_surface,
        extent
    );

    if (!m_swapchain->initialize()) {
        throw std::runtime_error("failed to create swap chain!");
    }

    return true;
}

void GraphicsDevice::destroy() {

    m_device.cleanup();

    if (m_surface) {
        vkDestroySurfaceKHR(m_instance.handle(), m_surface, nullptr); // TODO: Change this?
        m_surface = VK_NULL_HANDLE;
    }

    m_instance.cleanup();
}

core::Context GraphicsDevice::getContext() const {
    return core::Context {
        m_device.getDevice(),
        m_device.getPhysicalDevice(),
        m_device.getCommandPool(),
        m_device.getGraphicsQueue(),
        m_framesInFlight
    };
}

}; // namespace renderer::core