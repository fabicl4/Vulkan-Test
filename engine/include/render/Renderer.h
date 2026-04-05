#pragma once

#include <assert.h>
#include <memory>
#include <vector>

#include <core/types.h>
#include <core/Log.h>

// RESOURCE LAYER
#include <resources/Resource.h>
#include <resources/Shader.h>

#include "vulkan/Buffer.h"
#include "vulkan/Vertex.h"

#include "SubRenderer.h"

// fwd declarations
class Window;

class Device;
class SwapChain;
class Surface;
class Instance;

// Organice the frame and calls the subrenderers.
class Renderer {
    static const int MAX_FRAMES_IN_FLIGHT = 2;
public:
    Renderer(
        Window& window,
        ResourceSystem& resourceSystem
    ) : 
        m_window(window), 
        m_resourceSystem(resourceSystem)
    {
        createVkContext();
    }

    bool initialize();

    void cleanup();

    /*
        wait fence
        acquire image
        reset command buffer
        begin command buffer
    */
    void beginFrame();

    void draw();

    /*
        end command buffer
        submit
        present
    */
    void endFrame();

    void resize(int width, int height) {
        recreateSwapChain();
        // ...
    }

        void addSubRenderer(SubRenderer* subRenderer) {
        m_subRenderers.push_back(subRenderer);
    }

private:
    // handle events
    //-------------------------------------------------------------------------

    // Window event propagation:
    //  window -> application -> renderer
    void onWindowResize(int width, int height);

private:
    void createVkContext();
    void recreateSwapChain();
    void createRenderPass();
    void createFramebuffers(); // Create front and back framebuffers
    void createCommandBuffers();
    void createSyncObjects();

    VkResult acquireNextImage(VkSemaphore signalSemaphore, uint32_t* index);
    VkResult present(uint32_t imageIndex, VkSemaphore waitSemaphore);

private:
    Window& m_window;
    ResourceSystem& m_resourceSystem;

    bool m_framebufferResized = false;

    struct VkContext {
        ~VkContext() = default;

        Device& device() const { return *m_device; }
        SwapChain& swapchain() const { return *m_swapChain; }
        Surface& surface() const { return *m_surface; }

        Device* m_device;
        Instance* m_instance;
        SwapChain* m_swapChain;
        Surface* m_surface;
    } m_vkContext;

    
    // represent a frame in flight
    struct FrameContext {
        VkCommandBuffer cmd;

        VkFence fence;
        VkSemaphore imageAvailable;
        VkSemaphore renderFinished;
    };
    std::array<FrameContext, MAX_FRAMES_IN_FLIGHT> m_frameContext {};

    uint32_t m_imageIndex;

    // Front and back framebuffer
    std::vector<VkFramebuffer> m_framebuffers{};

    // current render buffer
    uint32_t m_currentFrame{0};

    VkRenderPass m_renderPass{};

    // SubRenderes
    std::vector<SubRenderer*> m_subRenderers{};
};

// SubRenderers

//Pipeline
//PipelineLayout
//DescriptorSets
//Buffers
// subcommand??