#include <platform/GLFW/GLFWSurface.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace platform {

GLFWSurface::~GLFWSurface() {}

VkSurfaceKHR GLFWSurface::createVulkanSurface(VkInstance instance) const {
    VkSurfaceKHR surface{};
    if (glfwCreateWindowSurface(instance, m_window, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create window surface");
    return surface;
}

void GLFWSurface::getFramebufferSize(int& width, int& height) const {
    glfwGetFramebufferSize(m_window, &width, &height);
}

}; // namespace platform