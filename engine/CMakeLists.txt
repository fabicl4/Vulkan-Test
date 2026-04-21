# engine/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(VKTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# external dependencies
# ------------------------
add_subdirectory(vendor)

# Engine modules
# ------------------------

add_subdirectory(Core)
add_subdirectory(Platform)
add_subdirectory(Renderer)
add_subdirectory(App)