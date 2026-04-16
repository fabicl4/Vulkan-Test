#pragma once

#include <vulkan/vulkan.h>

#include <assert.h>
#include <memory>
#include <vector>

#include <core/types.h>
#include <core/Log.h>

#include "vulkan/Context.h"

#include "RenderContext.h"
#include "FrameContext.h"

#include "pipeline/RenderPass.h"
#include "pipeline/RenderTarget.h"

#include <render/passes/TrianglePass.h>

// fwd declarations
class Window;

class Device;
class SwapChain;
class Surface;
class Instance;

// Organice the frame and calls the subrenderers.
class Renderer {
public:
    static const int MAX_FRAMES_IN_FLIGHT = 2;

    Renderer(Window* window);
    ~Renderer() = default;

    bool initialize();

    void cleanup();

    void drawFrame();

    void resize(int width, int height);

    // Vulkan context accessors (internal use only, be careful)
    Device* device() const { return m_device; }
    SwapChain* swapChain() const { return m_swapChain; }
    Surface* surface() const { return m_surface; }

    Context& context() { return m_context; }

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

private:
    /*
     *  wait fence
     *  acquire image
     *  reset command buffer
     *  begin command buffer
     * */
    void beginFrame();

    /*
     *  end command buffer
     *  submit
     *  present
     * */
    void endFrame();

private:
    void createRenderTarget();
    //void createRenderPass();

    // Swapchain render target (final output)
    // images and image views are not owned by the render target, they are 
    // owned by the swapchain. The render target just references them.
    RenderTarget m_renderTarget;
    
    TrianglePass* m_trianglePass;

private:
    // handle events
    //-------------------------------------------------------------------------

    // Window event propagation:
    //  window -> application -> renderer
    void onWindowResize(int width, int height);

private:
    // Graphics context
    Device* m_device;
    Instance* m_instance;
    SwapChain* m_swapChain;
    Surface* m_surface;

    Context m_context; // Vulkan context with device, queues, etc.

    void createContext();

private:

    void recreateSwapChain();

    void createCommandBuffers();
    void createSyncObjects();

    VkResult acquireNextImage(VkSemaphore signalSemaphore, uint32_t* index);
    VkResult present(uint32_t imageIndex, VkSemaphore waitSemaphore);

private:
    Window* m_window;

    bool m_framebufferResized = false;

    std::array<FrameContext, MAX_FRAMES_IN_FLIGHT> m_frameContext {};
    // current render buffer
    uint32_t m_currentFrame{0};

    uint32_t m_imageIndex;
};

// SubRenderers

//Pipeline
//PipelineLayout
//DescriptorSets
//Buffers
// subcommand??