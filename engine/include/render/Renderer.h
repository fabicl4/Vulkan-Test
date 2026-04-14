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
//#include "vulkan/Pipeline.h"

#include "RenderPass.h"
#include "RenderTarget.h"
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

    struct VkContext { // Graphic context
        Device* device;
        Instance* instance;
        SwapChain* swapChain;
        Surface* surface;
    } m_vkContext;
    
    // represent a frame in flight
    struct FrameContext {
        VkCommandBuffer cmd; // primary command buffer for the frame

        VkFence fence;
        VkSemaphore imageAvailable;
        VkSemaphore renderFinished;
    };

public:
    Renderer(Window* window);
    ~Renderer() = default;

    inline void AddSubRenderer(SubRenderer* subRenderer) {
        m_subRenderers.push_back(subRenderer);
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

    // Frame context accessorrs
    VkCommandBuffer getCurrentCommandBuffer() {
        return m_frameContext[m_currentFrame].cmd;
    }

    u32 getCurrentImageIndex() const {
        return m_imageIndex;
    }

    u32 getCurrentFrameIndex() const {
        return m_currentFrame;
    }

    // Vulkan context accessors
    Device* device() const { return m_vkContext.device; }
    SwapChain* swapChain() const { return m_vkContext.swapChain; }
    Surface* surface() const { return m_vkContext.surface; }

    RenderTarget getRenderTarget() const;

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
    Window* m_window;
    //ResourceSystem& m_resourceSystem;

    bool m_framebufferResized = false;

    std::array<FrameContext, MAX_FRAMES_IN_FLIGHT> m_frameContext {};
    // current render buffer
    uint32_t m_currentFrame{0};

    uint32_t m_imageIndex;

    // Front and back framebuffer
    std::vector<VkFramebuffer> m_framebuffers{};

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