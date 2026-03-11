#pragma once

#include "defines.h"

#include "Device.h"

// Backbuffers and presentantion
//
// SwapChain is in charge of:
//  - Create swapchain
//  - Create image views
//  - Recreate on resize
//  - Acquire image
//  - Present image
class SwapChain {
public:
    //static const char* kSwapChainExtension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    SwapChain(Device* device, VkSurfaceKHR surface, VkExtent2D extent)
    : m_device(device), m_surface(surface), m_extent(extent) {
        recreate(extent);
    }

    ~SwapChain() {}

    void initialize();
    void cleanup();

    // Delete copy semantics
    SwapChain(const SwapChain &) = delete;
    void operator=(const SwapChain &) = delete;

    //-------------------------------------------------------------------------
    // recreate swapchain after window resized.
    void recreate(VkExtent2D extent);

    // Intenta adquirir la siguiente imagen. Devuelve:
    // - VK_SUCCESS: imagen adquirida, índice en out_imageIndex
    // - VK_ERROR_OUT_OF_DATE_KHR: la swapchain no es compatible (el llamante debe recrear)
    // - Otros errores: lanza excepción o devuelve el código.
    VkResult acquireNextImage(VkSemaphore signalSemaphore, uint32_t* out_imageIndex);

    // Presenta la imagen índice 'imageIndex' esperando a que el semáforo esté listo.
    // Devuelve VK_SUCCESS, VK_SUBOPTIMAL_KHR o VK_ERROR_OUT_OF_DATE_KHR.
    VkResult present(uint32_t imageIndex, VkSemaphore waitSemaphore);

    //-------------------------------------------------------------------------
    // getters
    VkSwapchainKHR handle() const { return m_handle; }
    VkImageView getImageView(int index) { return m_imageViews[index]; }
    size_t imageCount() { return m_images.size(); }
    VkFormat getSwapChainImageFormat() { return m_imageFormat; }
    VkExtent2D getSwapChainExtent() { return m_extent; }
    uint32_t width() { return m_extent.width; }
    uint32_t height() { return m_extent.height; }

    float extentAspectRatio() {
        return static_cast<float>(m_extent.width) / 
            static_cast<float>(m_extent.height);
    }

private:

    // Member data creation functions
    void createSwapChain();
    void createImageViews();

    // helper functions
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes);
    
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkExtent2D chooseSwapExtent(
        const VkSurfaceCapabilitiesKHR& capabilities);

    Device* m_device;
    VkSurfaceKHR m_surface;
    
    // attributes
    VkSwapchainKHR m_handle{VK_NULL_HANDLE};
    std::vector<VkImage> m_images{};
    std::vector<VkImageView> m_imageViews{};

    VkFormat m_imageFormat{};
    VkExtent2D m_extent{};

};

