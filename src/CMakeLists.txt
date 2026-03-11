add_executable(vulkan_tutorial
    main.cpp
    Log.cpp
    Window.cpp
    Instance.cpp
    Device.cpp
    SwapChain.cpp
    VkContext.cpp
    Vertex.cpp
    Renderer.cpp
)

# Include directories
target_include_directories(vulkan_tutorial PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link external dependencies
target_link_libraries(vulkan_tutorial PRIVATE
    glfw
    glm
    Vulkan::Vulkan
    spdlog::spdlog
)