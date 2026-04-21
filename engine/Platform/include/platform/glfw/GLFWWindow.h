#pragma once

#include <string>
#include <memory>
#include <functional>

#include <core/Log.h>

#include <platform/IWindow.h>
#include <platform/ISurface.h>

// fwd
struct GLFWwindow;

namespace platform {

class GLFWWindow: public IWindow {
    using onWindowResizeCallback = std::function<void(int, int)>;
    using onKeyPressCallback     = std::function<void(int)>;
public:
    GLFWWindow(uint32_t width, uint32_t height, std::string name)
        : m_width(width), m_height(height), m_name(name) {}
    
    ~GLFWWindow() = default;
    
    // Create the GLFW window
    bool create();
    void destroy();

    bool shouldClose() const override;
    void pollEvents() const override;
    void getFramebufferSize(int& w, int& h) const override;

    std::unique_ptr<ISurface> createSurface() override;

    // Get the required extensions for Vulkan instance creation
    std::vector<const char*> getRequiredExtensions();

    inline void registerOnWindowResizeCallback(onWindowResizeCallback callback) {
        m_onWindowResizeCallback = callback;
    }

    bool isCreated() const {return m_isCreated && m_window != nullptr;};

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

}; // namespace platform