#engine/Core/CMakeLists.txt

# Vulkan
find_package(Vulkan REQUIRED)

add_library(VKTestCore STATIC
    src/Log.cpp
)

target_precompile_headers(VKTestCore PRIVATE src/pch.h)

target_include_directories(VKTestCore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(VKTestCore 
    PUBLIC
        spdlog::spdlog
        glm
        Vulkan::Vulkan
)

if(MSVC)
    target_compile_options(VKTestCore PRIVATE
        /W4
        /external:W0
    )

endif()