cmake_minimum_required(VERSION 3.15)

project(vulkan_tutorial LANGUAGES CXX)

find_package(Vulkan REQUIRED)

# find_package(glfw3 REQUIRED)

# Set global default configurations
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# extenal dependecies
add_subdirectory(vendor)

if (NOT TARGET spdlog::spdlog)
    message(FATAL_ERROR "Dependency spdlog not found!")
endif()

if (MSVC)
    add_compile_options(/FS)
endif()


add_subdirectory(src)