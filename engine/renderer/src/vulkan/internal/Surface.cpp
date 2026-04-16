#include <render/vulkan/internal/Surface.h>

#include <core/Log.h>

// create surface
bool Surface::createSurface(VkInstance instance, GLFWwindow* window) {
    // Implementation for creating surface
    if(glfwCreateWindowSurface(instance, window, nullptr, &m_surface) != VK_SUCCESS) {
        LOG_CRITICAL("[VkContext]Could NOT create the Surface");
        return false;
    }

    return true;
}

// destroy surface
void Surface::destroySurface(VkInstance instance) {
    // Implementation for destroying surface
    if (m_surface) {
        vkDestroySurfaceKHR(instance, m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }
}