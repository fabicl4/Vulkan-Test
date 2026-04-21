#include <application/Application.h>

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

        //onRender();

        m_renderer->drawFrame();

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
    m_window = std::make_unique<platform::GLFWWindow>(m_width, m_height, m_name);
    if (!m_window->create()) {
        return false;
    }
    
    m_surface = m_window->createSurface();

    renderer::GraphicsDeviceDesc desc = {};
    desc.extensions = m_window->getRequiredExtensions();

    m_gfxDevice = std::make_unique<renderer::GraphicsDevice>();
    if (!m_gfxDevice->create(*m_surface, desc)) {
        return false;
    }

    // Create the resource system
    //m_resourceSystem = std::make_unique<ResourceSystem>();
    //m_resourceSystem->registerManager<Shader>(new ShaderManager(...));

    // Systems
    //---------
    m_renderer = std::make_unique<renderer::Renderer> (
        // owned by the renderer
        *m_gfxDevice
    );

    if (!m_renderer->initialize()) {
        return false;
    }

    // ...

    // Events
    //---------
    // Register callback
    //m_window->registerOnWindowResizeCallback([this](int width, int height)
    //{
    //    //LOG_TRACE("[Application]Window resized: {}x{}", width, height);
    //    m_renderer->resize(width, height);
    //});

    return true;
}

bool Application::shutdown() {
    m_renderer->cleanup();
    m_renderer.reset();

    m_surface.reset();

    m_window->destroy();
    m_window.reset();

    Log::Shutdown();
    return true;
}