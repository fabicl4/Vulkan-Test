#include "VkContext.h"

VkContext::VkContext(Window* window, bool enableValidationLayers) 
    : m_window(window), m_enableValidationLayers(enableValidationLayers)
{
}

void VkContext::create() {
    // Create instance
    m_instance = std::make_unique<Instance>(m_enableValidationLayers);
    if (!m_instance->initialize()) {
        LOG_CRITICAL("[VkDevice]Could NOT create the Instance");
        return;
    }

    // Create surface
    if(glfwCreateWindowSurface(m_instance->handle(), m_window->getNativeHandle(), nullptr, &m_surface) != VK_SUCCESS) {
    //if (!m_window->createSurface(m_instance->handle(), &m_surface)) {
        LOG_CRITICAL("[VkContext]Could NOT create the Surface");
        return;
    }

    // Create Device
    m_device = std::make_unique<Device>(m_instance->handle(), m_surface);
    if (!m_device->initialize()) {
        LOG_CRITICAL("[VkDevice]Could NOT create the Device");
        return;
    }

    // Create SwapChain
    m_swapchain = std::make_unique<SwapChain>(
        m_device.get(),
        m_surface,
        m_window->getExtent()
    );
}