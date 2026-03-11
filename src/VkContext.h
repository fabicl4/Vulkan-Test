#pragma once

#include "defines.h"

#include "Window.h"

#include "Instance.h"
#include "Device.h"
#include "SwapChain.h"
#include "Pipeline.h"

class VkContext {
public:
    VkContext(Window* window, bool enableValidationLayers = false);
    
    void create();

    void destroy()
    {
        m_swapchain->cleanup();

        vkDestroySurfaceKHR(m_instance->handle(), m_surface, nullptr);

        m_device->cleanup();
        m_instance->cleanup();
    }


public:
    // Facede functions
    //-------------------------------------------------------------------------
    inline bool createBuffer(const BufferDescriptor& desc, Buffer& buffer) noexcept {
        return m_device->createBuffer(desc, buffer); 
    }
    inline void uploadData(Buffer& buffer, const void* data, VkDeviceSize size) noexcept {
        m_device->uploadData(buffer, data, size);
    }
    inline void releaseBuffer(Buffer& buffer) noexcept {
        m_device->releaseBuffer(buffer);
    };

    // Swapchain
    //-------------------------------------------------------------------------
    void recreateSwapchain(int width, int height) {
        VkExtent2D extent {};
        extent.width = static_cast<uint32_t>(width);
        extent.height = static_cast<uint32_t>(height);
        
        m_swapchain->recreate(extent);
    }

    VkResult acquireNextImage(VkSemaphore signalSemaphore, uint32_t* index) {
        return m_swapchain->acquireNextImage(signalSemaphore, index);
    }

    VkResult present(uint32_t imageIndex, VkSemaphore waitSemaphore) {
        return m_swapchain->present(imageIndex, waitSemaphore);
    }

    // Device
    //-------------------------------------------------------------------------
    void submitCommandBuffer(
        VkCommandBuffer cmd,
        VkSemaphore wait,
        VkSemaphore signal,
        VkFence fence
    ) {
        m_device->submitCommandBuffer(cmd, wait, signal, fence);
    }

    Pipeline* createPipeline(
        const std::vector<char>& vsCode, const std::vector<char>& fsCode,
        const PipelineConfigInfo& pipelineInfo
    ){
        return new Pipeline(m_device.get(), vsCode, fsCode, pipelineInfo);
    }

    // Accessors
    //-------------------------------------------------------------------------
    Device& device() { return *m_device; }
    SwapChain& swapchain() { return *m_swapchain; }

private:
    Window* m_window;
    bool m_enableValidationLayers;
    
    std::unique_ptr<Instance> m_instance;
    std::unique_ptr<Device> m_device;
    std::unique_ptr<SwapChain> m_swapchain;

    VkSurfaceKHR m_surface;
};