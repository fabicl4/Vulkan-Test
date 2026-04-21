#include <platform/GLFW/GLFWWindow.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <platform/GLFW/GLFWSurface.h>

namespace platform {

bool GLFWWindow::shouldClose() const {
    return glfwWindowShouldClose(m_window);
};

void GLFWWindow::pollEvents() const {
    glfwPollEvents();
}

void GLFWWindow::getFramebufferSize(int& w, int& h) const {
    w = static_cast<int>(m_width);
    h = static_cast<int>(m_height);
}

std::unique_ptr<ISurface> GLFWWindow::createSurface() {
    return std::make_unique<GLFWSurface>( getNativeHandle() );
}

bool GLFWWindow::create() {
    if (!glfwInit()) {
        LOG_ERROR("[GLFWWindow::create]Could NOT initialize GLFW!");
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);
    
    if (!m_window) {
        LOG_ERROR("[GLFWWindow::create]Failed to create the window!");
        return false;
    }

    m_isCreated = true;

    // register callbacks
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, [] (GLFWwindow* window, int width, int height) {
        auto self = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));

        self->m_width  = static_cast<uint32_t>(width);
        self->m_height = static_cast<uint32_t>(height);

        LOG_TRACE("[GLFWWindow] Resize window {} {}", width, height);

        if (self->m_onWindowResizeCallback) {
            self->m_onWindowResizeCallback(width, height);
        }
    });

    return true;
}

void GLFWWindow::destroy() {
    if (!isCreated()) {
        return;
    }

    glfwDestroyWindow(m_window);
    m_window = nullptr;
    m_isCreated = false;

    glfwTerminate();
}

std::vector<const char*> GLFWWindow::getRequiredExtensions() {
    uint32_t count = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&count);

    return std::vector<const char*>(glfwExtensions, glfwExtensions + count);
}

}; // namespace platform