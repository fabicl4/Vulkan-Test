#engine/App/CMakeLists.txt

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(VKTestApp STATIC
    ${SOURCE_DIR}/Application.cpp
)

target_include_directories(VKTestApp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(VKTestApp
    PUBLIC
        VKTestCore
        VKTestPlatform
        VKTestRenderer
    PRIVATE
)