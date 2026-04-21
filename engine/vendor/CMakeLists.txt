include(FetchContent)

# GLFW
add_subdirectory(glfw-3.4)

# GLM (header-only)
add_library(glm INTERFACE)

target_include_directories(glm INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/glm
)

# STB
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)

# SPDLOG
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0
)

FetchContent_MakeAvailable(spdlog)

add_library(spdlog_system INTERFACE)

target_link_libraries(spdlog_system INTERFACE spdlog::spdlog)

target_include_directories(spdlog_system SYSTEM INTERFACE
    $<TARGET_PROPERTY:spdlog::spdlog,INTERFACE_INCLUDE_DIRECTORIES>
)