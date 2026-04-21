#pragma once

#include <string>
#include <memory>
#include <vector>

// CORE LAYER
#include <core/Log.h>

#include <platform/glfw/GLFWWindow.h>
#include <platform/glfw/GLFWSurface.h>

#include <renderer/GraphicsDevice.h>
#include <renderer/Renderer.h>

class Window;
class Renderer;

class Application
{
public:
    Application(int width, int height, std::string name) 
    : m_width(width), m_height(height), m_name(name) {} 

    bool run();

protected:
    virtual bool onCreate() { return true; };
    virtual bool onDestroy() { return true; };
    // ts: time step / delta time in seconds since last update
    virtual bool onUpdate(double ts) { return true; };
    virtual bool onRender() { return true; };
    virtual bool onEvent() { return true; };

private: 

    // Initialize systems
    bool initialze();

    bool shutdown();

protected:
    // Systems
    std::unique_ptr<platform::GLFWWindow> m_window;
    std::unique_ptr<platform::ISurface> m_surface;

    std::unique_ptr<renderer::GraphicsDevice> m_gfxDevice;
    std::unique_ptr<renderer::Renderer> m_renderer;

private:

    int m_width;
    int m_height; 
    std::string m_name;
    
    // AudioSystem
    // UISystem
    
    // Layers
    // ...
};

/*
    Window → inicializa GLFW y crea la ventana
    Renderer ->
        Context -> [Si quisieramos tener multiples ventanas deberíamos de separalos]
            Instance → crea Vulkan instance
            Device → selecciona GPU y crea logical device
            SwapChain → usa Device + Surface
        
*/