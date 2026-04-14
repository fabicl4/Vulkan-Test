# engine/CMakeLists.txt

cmake_minimum_required(VERSION 3.20)
project(VkCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# static library
add_library(VkCore STATIC)

if (MSVC)
    target_compile_options(VkCore PRIVATE /W4)
else()
    target_compile_options(VkCore PRIVATE -Wall -Wextra -Wpedantic)
endif()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

target_sources(VkCore
    PRIVATE
        ${SRC_FILES}
)

target_include_directories(VkCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_precompile_headers(VkCore PRIVATE
    src/pch.h
)

# Vulkan
find_package(Vulkan REQUIRED)

# external dependencies
# ------------------------
# GLFW
add_subdirectory(vendor)

# Link external dependencies
target_link_libraries(VkCore
    PUBLIC
        glm
        Vulkan::Vulkan
        spdlog::spdlog
    PRIVATE
        glfw
        stb
        
)