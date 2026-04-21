#include <renderer/Renderer.h>

namespace renderer {

bool Renderer::initialize ()
{   
    // TODO: manage fail states..
    
    m_ctx = m_device.getContext(); 

    VkExtent2D windowExtent = m_device.extent();

    // create render passes
    //m_trianglePass = new TrianglePass(*m_device);

    // create render target (references swapchain images)
    createRenderTarget();

    // initialize render passes (create render pass, create framebuffers, etc)
    //if (!m_trianglePass->initialize(m_renderTarget)) {
    //    throw std::runtime_error("failed to create triangle render pass!");
    //}

    createCommandBuffers();
    createSyncObjects();

    return true;
}

void Renderer::cleanup() {
    vkDeviceWaitIdle(m_ctx.device);

    // frame context
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(m_ctx.device, m_frameContext[i].imageAvailable, nullptr);
        vkDestroySemaphore(m_ctx.device, m_frameContext[i].renderFinished, nullptr);
        vkDestroyFence(m_ctx.device, m_frameContext[i].fence, nullptr);
    }

    // release render passes
    //-----------------------------------------------------------------------------
    //m_trianglePass->cleanup();
    //delete m_trianglePass;

    //m_swapChain->cleanup();
    //m_device->cleanup();
    //m_surface->destroySurface(m_instance->handle());
    //m_instance->cleanup();
}


void Renderer::drawFrame() {
    beginFrame();

    /*
    // Record commands for the current frame
    m_trianglePass->prepare();
    VkCommandBuffer cmdBuffer = getCurrentCommandBuffer(); // Get the command buffer for the current frame
    u32 imageIndex = getCurrentImageIndex(); // Get the index of the current swapchain image
    u32 frameIndex = getCurrentFrameIndex(); // Get the index of the current frame
    VkExtent2D extent = m_renderTarget.extent; // Get the extent of the render target
    
    m_trianglePass->execute(cmdBuffer, imageIndex, frameIndex, extent); // Execute the triangle pass
    */
    endFrame();
}

void Renderer::resize(int width, int height) {
    m_framebufferResized = true;
}

void Renderer::beginFrame()
{
    auto& frame = m_frameContext[m_currentFrame];
    VkCommandBuffer commandBuffer = m_frameContext[m_currentFrame].cmd;

    // wait fence
    vkWaitForFences( //TODO: Encapsulate
        m_ctx.device, 
        1, 
        &frame.fence,
        VK_TRUE, 
        UINT64_MAX
    );

    // acquire image
    VkResult result = acquireNextImage(frame.imageAvailable, &m_imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // reset fences
    vkResetFences(m_ctx.device, 1,
                &frame.fence);
    // reset command buffer
    vkResetCommandBuffer(frame.cmd, 0); /*VkCommandBufferResetFlagBits*/

    // begin command buffer ...
    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    
}

VkResult Renderer::submitCommandBuffer(
        VkCommandBuffer cmd,
        VkSemaphore waitSemaphore,
        VkSemaphore signalSemaphore,
        VkFence fence
) {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {waitSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    VkCommandBuffer commandBuffers[] = {cmd};
    submitInfo.pCommandBuffers = commandBuffers;

    VkSemaphore signalSemaphores[] = {signalSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    return vkQueueSubmit(m_ctx.graphicsQueue, 1, &submitInfo, fence);
}

/*
    end command buffer
    submit
    present
*/
void Renderer::endFrame() {

    auto& frame = m_frameContext[m_currentFrame];
    VkCommandBuffer commandBuffer = frame.cmd;

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    // submit
    if (submitCommandBuffer(
        frame.cmd,
        frame.imageAvailable,
        frame.renderFinished,
        frame.fence
    ) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // present
    VkResult result = present(m_imageIndex, m_frameContext[m_currentFrame].renderFinished);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized) {
        m_framebufferResized = false;
        //recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

//-----------------------------------------------------------------------------
/*
void Renderer::createRenderTarget() {
    m_renderTarget.colorAttachments.clear();

    m_renderTarget.extent = m_swapChain->getExtent();

    // Create color attachments from swapchain image views
    for (size_t i = 0; i < m_swapChain->imageCount(); i++) {
        Texture colorAttachment{};

        colorAttachment.image = m_swapChain->getImage(i);
        colorAttachment.imageView = m_swapChain->getImageView(i);
        colorAttachment.sampler = VK_NULL_HANDLE; // No sampler needed for render target

        colorAttachment.format = m_swapChain->getImageFormat();
        colorAttachment.extent = m_swapChain->getExtent();

        colorAttachment.width  = colorAttachment.extent.width;
        colorAttachment.height = colorAttachment.extent.height;

        colorAttachment.mipmaps = 1;

        colorAttachment.debugName = "SwapchainImage_" + std::to_string(i);

        m_renderTarget.colorAttachments.push_back(colorAttachment);
    }

    m_renderTarget.isSwapChainTarget = true;
}

*/

//-----------------------------------------------------------------------------
void Renderer::createCommandBuffers() {
    VkCommandPool commandPool = m_ctx.commandPool;

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; 
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    // 3. Crear un array con los handles de los command buffers
    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> cmdBuffers;
    if (vkAllocateCommandBuffers(m_ctx.device, &allocInfo, cmdBuffers.data()) != VK_SUCCESS) {
        return;
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_frameContext[i].cmd = cmdBuffers[i];
    }
}

void Renderer::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(m_ctx.device, &semaphoreInfo, nullptr, &m_frameContext[i].imageAvailable) != VK_SUCCESS ||
                vkCreateSemaphore(m_ctx.device, &semaphoreInfo, nullptr, &m_frameContext[i].renderFinished) != VK_SUCCESS ||
                vkCreateFence(m_ctx.device, &fenceInfo, nullptr, &m_frameContext[i].fence) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
}

/*
void Renderer::recreateSwapChain() {
    // Wait to GPU to finish drawing and then we recreate the SwapChain
    vkDeviceWaitIdle(m_ctx.device);

    u32 width = m_window->GetWidth();
    u32 height = m_window->GetHeight();
    VkExtent2D windowExtent = {width, height};

    m_swapChain->recreate(windowExtent);
    
    createRenderTarget(); // Recreate render target with new swapchain images

    m_trianglePass->resize(m_renderTarget);
}


VkResult Renderer::acquireNextImage(VkSemaphore signalSemaphore, uint32_t* index) {
    return m_swapChain->acquireNextImage(signalSemaphore, index);
}
VkResult Renderer::present(uint32_t imageIndex, VkSemaphore waitSemaphore) {
    return m_swapChain->present(imageIndex, waitSemaphore);
}
*/

};