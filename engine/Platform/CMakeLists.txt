#engine/Platform/CMakeLists.txt

add_library(VKTestPlatform STATIC
#    src/GLFWSurface.cpp
    src/GLFWWindow.cpp
)

target_include_directories(VKTestPlatform PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(VKTestPlatform
    PUBLIC
        VKTestCore
    PRIVATE
        glfw
        
)