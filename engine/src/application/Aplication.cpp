#include <application/Application.h>

#include <platform/window/Window.h>

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

        m_renderer->beginFrame();
        m_renderer->draw();
        m_renderer->endFrame();

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
    m_window = std::make_unique<Window>(800, 600, "Vulkan Test");
    if(!m_window || !m_window->create())
    {
        return false;
    }

    // Create the resource system
    m_resourceSystem = std::make_unique<ResourceSystem>();
    //m_resourceSystem->registerManager<Shader>(new ShaderManager(...));

    // Systems
    //---------
    m_renderer = std::make_unique<Renderer>(*m_window, *m_resourceSystem);
    if (!m_renderer->initialize()) {
        return false;
    }

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
    m_window.reset();

    Log::Shutdown();
    return true;
}