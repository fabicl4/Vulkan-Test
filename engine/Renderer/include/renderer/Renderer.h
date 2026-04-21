#pragma once

#include <vulkan/vulkan.h>

#include <assert.h>
#include <memory>
#include <vector>

#include <core/types.h>
#include <core/Log.h>

#include "GraphicsDevice.h"

#include <renderer/core/Context.h>

// Context
//#include "RenderContext.h"
#include <renderer/core/FrameContext.h>

// Pipeline
#include <renderer/RenderPass.h>
#include <renderer/RenderTarget.h>

// Hardcoded passes
//#include <renderer/passes/TrianglePass.h>

// fwd declarations
// ...

namespace renderer {

enum class RenderMode {
    HEADLESS, // for offscreen rendering, compute, etc
    WINDOWED, // for rendering to a window (swapchain)

};

// Organice the frame
class Renderer {
public:
    static const int MAX_FRAMES_IN_FLIGHT = 2;

    explicit Renderer(GraphicsDevice& device) 
        : m_device(device) {};
    ~Renderer() = default;

    bool initialize();

    void cleanup();

    void drawFrame();

    void resize(int width, int height);

public:

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

    VkResult submitCommandBuffer(
        VkCommandBuffer cmd,
        VkSemaphore waitSemaphore,
        VkSemaphore signalSemaphore,
        VkFence fence
    );

    // handle events
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    void createRenderTarget();
    void recreateSwapChain();

    void createCommandBuffers();
    void createSyncObjects();

    VkResult acquireNextImage(VkSemaphore signalSemaphore, uint32_t* index);
    VkResult present(uint32_t imageIndex, VkSemaphore waitSemaphore);

private:
    GraphicsDevice& m_device;   // no owning
    core::Context m_ctx;

    // Swapchain render target (final output)
    // images and image views are not owned by the render target, they are 
    // owned by the swapchain. The render target just references them.
    RenderTarget m_renderTarget;
    
    // TrianglePass* m_trianglePass;

    bool m_framebufferResized = false;

    // Resources frame-in-flight
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

}; // namespace renderer