#pragma once

#include <string>

#include "Renderer.h"
#include "Window.h"
#include "Log.h"

/*
Application
 ├── Window
 ├── VulkanContext
 │    ├── Instance
 │    ├── Device
 │    └── Swapchain
 └── Renderer
*/

class Application
{
public:
    Application(int width, int height, std::string name) 
    {} 

    bool Run() {
        if (!initialze()) {
            LOG_ERROR("[Application]Could not initialize");
            
            return false;
        }

        LOG_TRACE("[Application]Entering main loop");

        if (!onCreate()) {
            LOG_ERROR("[Application]Failed to create the user application");
            return false;
        }

        while (!m_window->shouldClose())
        {
            // process input
            // ...

            m_renderer->beginFrame();
            m_renderer->record();
            m_renderer->endFrame();

            onUpdate(0);

            m_window->pollEvents();
        }

        if (!onDestroy()) {
            LOG_ERROR("[Application]Failed to destroy application the user application");
        }

        return shutdown();
    }

protected:
    virtual bool onCreate() { return true; };
    virtual bool onDestroy() { return true; };
    // ts: time step / delta time in seconds since last update
    virtual bool onUpdate(double ts) { return true; };
    virtual bool onRender() { return true; };

private: 

    // Initialize systems
    bool initialze() {
        Log::Init();

        m_window = new Window(800, 600, "Vulkan Test");

        if(!m_window || !m_window->create())
        {
            return false;
        }

        m_renderer = new Renderer(m_window);
        m_renderer->initialize();

        return true;
    }

    bool shutdown() {
        m_window->destroy();
        Log::Shutdown();
        return true;
    }

private:
    Window* m_window;
    Renderer* m_renderer;
};

/*
    Window → crea superficie
    Instance → crea Vulkan instance
    Device → selecciona GPU y crea logical device
    SwapChain → usa Device + Surface
*/