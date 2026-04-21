# sandbox/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(Sandbox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Sandbox src/main.cpp)

target_include_directories(Sandbox PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src     # Sandbox headers if any
)

target_link_libraries(Sandbox 
    PRIVATE 
        VKTestApp
)