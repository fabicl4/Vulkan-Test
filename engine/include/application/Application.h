#pragma once

#include <string>
#include <memory>

// CORE LAYER
#include <core/Log.h>

// RESOURCE LAYER
#include <resources/Resource.h>
#include <resources/Shader.h>
//#include <resources/Texture.h>

// RENDERING LAYER
#include <render/Renderer.h>

/*
Application
 ├── Window
 ├── VulkanContext
 │    ├── Instance
 │    ├── Device
 │    └── Swapchain
 ├── ResourceManager
 └── Renderer
*/

class Window;

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
    Window* m_window;
    
    std::unique_ptr<Renderer> m_renderer {};
    std::unique_ptr<ResourceSystem> m_resourceSystem {};

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