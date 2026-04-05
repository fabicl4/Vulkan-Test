#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <functional>

#include <core/Log.h>

class Window {
    using onWindowResizeCallback = std::function<void(int, int)>;
    using onKeyPressCallback     = std::function<void(int)>;
public:
    Window(uint32_t width, uint32_t height, std::string name)
        : m_width(width), m_height(height), m_name(name) {}
    
    // Create the GLFW window
    // Create the surface
    bool create();

    bool createSurface(VkInstance instance, VkSurfaceKHR* surface);

    inline void registerOnWindowResizeCallback(onWindowResizeCallback callback) {
        m_onWindowResizeCallback = callback;
    }

    void destroy() {
        if (!isCreated()) {
            return;
        }

        glfwDestroyWindow(m_window);
        m_window = nullptr;
        m_isCreated = false;

        glfwTerminate();
    }

    bool isCreated() const {return m_isCreated && m_window != nullptr;};
    bool shouldClose() const {
        return glfwWindowShouldClose(m_window);
    };

    void pollEvents() {
        glfwPollEvents();
    }

    GLFWwindow* getNativeHandle() {return m_window;}
    VkExtent2D getExtent() const {
        VkExtent2D extent = {m_width, m_height};
        return extent;
    };

    uint32_t GetWidth() const { return m_width; }
    uint32_t GetHeight() const { return m_height; }


private:
    // Is only true if the window could be created, otherwise its false. 
    bool m_isCreated = false;

    onWindowResizeCallback m_onWindowResizeCallback;

    uint32_t m_width;
    uint32_t m_height;

    std::string m_name;

    GLFWwindow* m_window;
};