#engine/Renderer/CMakeLists.txt

# Vulkan
find_package(Vulkan REQUIRED)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(VKTestRenderer STATIC
    ${SOURCE_DIR}/core/Device.cpp
    ${SOURCE_DIR}/core/Instance.cpp
    
    ${SOURCE_DIR}/core/SwapChain.cpp

#    ${SOURCE_DIR}/vulkan/BufferAllocator.cpp

#    ${SOURCE_DIR}/utils/ShaderLoader.cpp
    ${SOURCE_DIR}/resources/PipelineBuilder.cpp
#    ${SOURCE_DIR}/utils/TextureLoader.cpp

    ${SOURCE_DIR}/RenderPass.cpp
    ${SOURCE_DIR}/types/Vertex.cpp
#    ${SOURCE_DIR}/pipeline/RenderPipeline.cpp

#    ${SOURCE_DIR}/passes/TrianglePass.cpp

    ${SOURCE_DIR}/Renderer.cpp
    ${SOURCE_DIR}/stb.cpp
)

target_include_directories(VKTestRenderer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

#target_include_directories(VKTestRenderer PUBLIC
#    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#)

target_link_libraries(VKTestRenderer
    PUBLIC
        Vulkan::Vulkan
        VKTestCore
    PRIVATE
        VKTestPlatform
        stb
)