#pragma once

#include <string>
#include <functional>

#include <core/Log.h>

#include <GLFW/glfw3.h>

// fwd
class GLFWwindow;

class Window {
    using onWindowResizeCallback = std::function<void(int, int)>;
    using onKeyPressCallback     = std::function<void(int)>;
public:
    Window(uint32_t width, uint32_t height, std::string name)
        : m_width(width), m_height(height), m_name(name) {}
    
    // Create the GLFW window
    
    bool create();
    void destroy();

    // Create the surface
    //bool createSurface(VkInstance instance, VkSurfaceKHR* surface);

    inline void registerOnWindowResizeCallback(onWindowResizeCallback callback) {
        m_onWindowResizeCallback = callback;
    }

    bool isCreated() const {return m_isCreated && m_window != nullptr;};
    bool shouldClose() const;
    void pollEvents();

    GLFWwindow* getNativeHandle() {return m_window;}

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