#include <application/Application.h>

#include <platform/window/Window.h>

#include "render/TestRenderer.h"

bool Application::run() {
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

        onUpdate(0);

        m_window->pollEvents();
    }

    if (!onDestroy()) {
        LOG_ERROR("[Application]Failed to destroy application the user application");
    }

    return shutdown();
}

bool Application::initialze() {
    // Initialize Logging system
    Log::Init();

    // Create the window
    m_window = new Window (m_width, m_height, m_name);
    if(!m_window || !m_window->create())
    {
        return false;
    }

    // Create the resource system
    m_resourceSystem = std::make_unique<ResourceSystem>();
    //m_resourceSystem->registerManager<Shader>(new ShaderManager(...));

    // Systems
    //---------
    m_renderer = std::make_unique<Renderer>(m_window);
    if (!m_renderer->initialize()) {
        return false;
    }

    //m_renderer->AddSubRenderer(new TestRenderer());

    // audio system
    // ...

    // Events
    //---------
    // Register callback
    m_window->registerOnWindowResizeCallback([this](int width, int height)
    {
        //LOG_TRACE("[Application]Window resized: {}x{}", width, height);
        m_renderer->resize(width, height);
    });

    return true;
}

bool Application::shutdown() {
    m_renderer->cleanup();
    m_renderer.reset();

    m_window->destroy();
    delete m_window;

    Log::Shutdown();
    return true;
}