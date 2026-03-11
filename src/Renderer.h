#pragma once

#include "defines.h"
#include "VkContext.h"
#include "Buffer.h"
#include "Vertex.h"
#include "Pipeline.h"

// Organice the frame and calls the subrenderers.
class Renderer {
public:
    Renderer(Window* window) 
        : m_window(window), m_vkContext(window, true)
    {}

    bool initialize()
    {
        assert(m_window != nullptr && "Cannot create Vulkan Graphic Context because window is null!");

        // Register callback
        //m_window->registerOnWindowResizeCallback(Renderer::onWindowResize);

        m_vkContext.create();
        // manage fail states...

        createRenderPass();
        createFramebuffers();
        createGraphicsPipeline();
        createVertexBuffer();
        createIndexBuffer();
        createCommandBuffers();
        createSyncObjects();
        
        return true;
    }

    void cleanup() {
        
        // ...
        m_vkContext.releaseBuffer(m_vertexBuffer);
        m_vkContext.releaseBuffer(m_indexBuffer);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(m_vkContext.device().getDevice(), m_frameContext[i].imageAvailable, nullptr);
            vkDestroySemaphore(m_vkContext.device().getDevice(), m_frameContext[i].renderFinished, nullptr);
            vkDestroyFence(m_vkContext.device().getDevice(), m_frameContext[i].fence, nullptr);
        }

        // release pipeline
        //m_pipeline.release();

        // release framebuffers
        for (auto framebuffer : m_framebuffers) {
            vkDestroyFramebuffer(m_vkContext.device().getDevice(), framebuffer, nullptr);
        }

        // release render pass
        vkDestroyRenderPass(
            m_vkContext.device().getDevice(), 
            m_renderPass, nullptr);

        // context cleanup
        m_vkContext.destroy();

        // ...
    }

    /*
        wait fence
        acquire image
        reset command buffer
        begin command buffer
    */
    void beginFrame()
    {
        // wait fence
        vkWaitForFences( //TODO: Encapsulate
            m_vkContext.device().getDevice(), 
            1, 
            &m_frameContext[m_currentFrame].fence,
            VK_TRUE, 
            UINT64_MAX
        );

        // acquire image
        VkResult result = m_vkContext.acquireNextImage(m_frameContext[m_currentFrame].imageAvailable, &m_imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        // reset fences
        vkResetFences(m_vkContext.device().getDevice(), 1,
                  &m_frameContext[m_currentFrame].fence);
        // reset command buffer
        vkResetCommandBuffer(m_frameContext[m_currentFrame].cmd, 0); /*VkCommandBufferResetFlagBits*/

        // begin command buffer ...
        /*
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(m_frameContext[m_currentFrame].cmd, &beginInfo);
        */
    }

    void record()
    {
        VkCommandBuffer commandBuffer = m_frameContext[m_currentFrame].cmd;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_framebuffers[m_imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_window->getExtent();

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
            //m_pipeline->bind(commandBuffer);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float) m_window->getExtent().width;
            viewport.height = (float) m_window->getExtent().height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = m_window->getExtent();
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);            

            VkBuffer vertexBuffers[] = {m_vertexBuffer.handle};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer.handle, 0, VK_INDEX_TYPE_UINT16);

            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    /*
        end command buffer
        submit
        present
    */
    void endFrame() {
        // end command buffer ...
        //vkEndCommandBuffer(m_frameContext[m_currentFrame].cmd);

        // submit
        if (m_vkContext.device().submitCommandBuffer(
            m_frameContext[m_currentFrame].cmd,
            m_frameContext[m_currentFrame].imageAvailable,
            m_frameContext[m_currentFrame].renderFinished,
            m_frameContext[m_currentFrame].fence
        ) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        // present
        VkResult result = m_vkContext.present(m_imageIndex, m_frameContext[m_currentFrame].renderFinished);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized) {
            m_framebufferResized = false;
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }


private:
    // handle events
    //-------------------------------------------------------------------------

    // Window event propagation:
    //  window -> application -> renderer
    void onWindowResize(int width, int height) {
        recreateSwapChain();
        // ...
    }

private:
    // frame functions

private:
    void recreateSwapChain() {
        // Wait to GPU to finish drawing and then we recreate the SwapChain
        vkDeviceWaitIdle(m_vkContext.device().getDevice());

        // 1. destroy front and back frame buffers
        for (auto framebuffer : m_framebuffers) {
            vkDestroyFramebuffer(m_vkContext.device().getDevice(), framebuffer, nullptr);
        }
        // 2. cleanup swapchain
        // 3. recreate swapchain
        m_vkContext.swapchain().recreate(m_window->getExtent());
        // 4. recreate front and back framebuffers
        createFramebuffers();
    }

    void createRenderPass();
    void createGraphicsPipeline();
    // Create front and back framebuffers
    void createFramebuffers();
    void createVertexBuffer();
    void createIndexBuffer();
    void createCommandBuffers();
    void createSyncObjects();

    VkShaderModule createShaderModule(const std::vector<char>& code);

private:
    VkContext m_vkContext;
    Window* m_window;

    bool m_framebufferResized = false;

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
    
    //Pipeline* m_pipeline;

    VkPipeline m_pipeline;
    VkPipelineLayout m_pipelineLayout;
    
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };


    // Buffers
    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;

    int m_numIndices = 6;

    // SubRenderes
    // ...
};

class SubRenderer 
{
private:
    //Pipeline
    //PipelineLayout
    //DescriptorSets
    //Buffers

    // subcommand??
};

class SpriteRenderer {};
class TextRenderer {};