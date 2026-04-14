#include <platform/window/Window.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

bool Window::create() {
    if (!glfwInit()) {
        LOG_ERROR("[Window::create]Could NOT initialize GLFW!");
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);
    
    if (!m_window) {
        LOG_ERROR("[Window::create]Failed to create the window!");
        return false;
    }

    m_isCreated = true;

    // register callbacks
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, [] (GLFWwindow* window, int width, int height) {
        auto self = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

        self->m_width  = static_cast<uint32_t>(width);
        self->m_height = static_cast<uint32_t>(height);

        LOG_TRACE("[Window] Resize window {} {}", width, height);

        if (self->m_onWindowResizeCallback) {
            self->m_onWindowResizeCallback(width, height);
        }
    });

    return true;
}

void Window::destroy() {
    if (!isCreated()) {
        return;
    }

    glfwDestroyWindow(m_window);
    m_window = nullptr;
    m_isCreated = false;

    glfwTerminate();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
};

void Window::pollEvents() {
    glfwPollEvents();
}

/*
bool Window::createSurface(VkInstance instance, VkSurfaceKHR* surface) {
    // Create the surface
    if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS) {
        LOG_ERROR("[Window::create]Failed to create window surface!");
        return false;
    }
    
    return true;
}
*/