#include <render/Renderer.h>

#include <vulkan/vulkan.h>

#include <platform/window/Window.h>

#include "vulkan/Device.h"
#include "vulkan/SwapChain.h"
#include "vulkan/Surface.h"
#include "vulkan/Instance.h"

Renderer::Renderer(
    Window* window) : 
    m_window(window)
{
}

bool Renderer::initialize()
{   
    // manage fail states...

    createContext();

    m_trianglePass = new TrianglePass(*m_device);

    createRenderTarget();

    if (!m_trianglePass->initialize(m_renderTarget)) {
        throw std::runtime_error("failed to create triangle render pass!");
    }

    //createFramebuffers();
    createCommandBuffers();
    createSyncObjects();

    return true;
}

void Renderer::cleanup() {
    vkDeviceWaitIdle(m_device->getDevice());

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(m_device->getDevice(), m_frameContext[i].imageAvailable, nullptr);
        vkDestroySemaphore(m_device->getDevice(), m_frameContext[i].renderFinished, nullptr);
        vkDestroyFence(m_device->getDevice(), m_frameContext[i].fence, nullptr);
    }

    // release render target frame buffers
    for (auto framebuffer : m_renderTarget.framebuffers) {
        vkDestroyFramebuffer(m_device->getDevice(), framebuffer, nullptr);
    }

    // release render passes
    //-----------------------------------------------------------------------------
    m_trianglePass->cleanup();
    delete m_trianglePass;

    // ...

    m_swapChain->cleanup();
    m_device->cleanup();
    m_surface->destroySurface(m_instance->handle());
    m_instance->cleanup();
}


void Renderer::drawFrame() {
    beginFrame();

    // Record commands for the current frame
    m_trianglePass->prepare();
    VkCommandBuffer cmdBuffer = getCurrentCommandBuffer(); // Get the command buffer for the current frame
    u32 imageIndex = getCurrentImageIndex(); // Get the index of the current swapchain image
    VkExtent2D extent = m_renderTarget.extent; // Get the extent of the render target
    
    m_trianglePass->execute(cmdBuffer, imageIndex, extent); // Execute the triangle pass

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
        m_device->getDevice(), 
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
    vkResetFences(m_device->getDevice(), 1,
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
    if (m_device->submitCommandBuffer(
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
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::createContext() {
    /*
    #if(DEBUG)
        bool enableValidationLayers = true;
    #else
        bool enableValidationLayers = false;
    #endif
    */

    bool enableValidationLayers = true;

    LOG_INFO("Creating Vulkan context...");
    LOG_INFO("Validation layers: {}", enableValidationLayers ? "enabled" : "disabled");

    m_instance = new Instance(enableValidationLayers);
    if (!m_instance->initialize()) {
        throw std::runtime_error("failed to create Vulkan instance!");
    }
    
    m_surface = new Surface();
    if (!m_surface->createSurface(
        m_instance->handle(), (GLFWwindow*)m_window->getNativeHandle())) {
        throw std::runtime_error("failed to create window surface!");
    }

    m_device = new Device(
        m_instance->handle(), 
        m_surface->getSurface()
    );
    if (!m_device->initialize()) {
        throw std::runtime_error("failed to create logical device!");
    }

    u32 width = m_window->GetWidth();
    u32 height = m_window->GetHeight();
    VkExtent2D windowExtent = {width, height};

    m_swapChain = new SwapChain(
        m_device, 
        m_surface->getSurface(), 
        windowExtent
    );

    if (!m_swapChain->initialize()) {
        throw std::runtime_error("failed to create swap chain!");
    }
}

#pragma region EventHandlers

void Renderer::onWindowResize(int width, int height) {
    resize(width, height);
}

#pragma endregion
//-----------------------------------------------------------------------------

void Renderer::createRenderTarget() {
    m_renderTarget.extent = m_swapChain->getExtent();
    m_renderTarget.colorFormat = m_swapChain->getImageFormat();

    // create framebuffers
    m_renderTarget.framebuffers.resize(m_swapChain->imageCount());

    for (size_t i = 0; i < m_swapChain->imageCount(); i++) {
        VkImageView imageView = m_swapChain->getImageView(i);

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_trianglePass->getVkRenderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &imageView;
        framebufferInfo.width = m_renderTarget.extent.width;
        framebufferInfo.height = m_renderTarget.extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_device->getDevice(), &framebufferInfo, nullptr, &m_renderTarget.framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

//-----------------------------------------------------------------------------
void Renderer::createCommandBuffers() {
    VkCommandPool commandPool = m_device->getCommandPool();

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; 
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    // 3. Crear un array con los handles de los command buffers
    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> cmdBuffers;
    if (vkAllocateCommandBuffers(m_device->getDevice(), &allocInfo, cmdBuffers.data()) != VK_SUCCESS) {
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
            if (vkCreateSemaphore(m_device->getDevice(), &semaphoreInfo, nullptr, &m_frameContext[i].imageAvailable) != VK_SUCCESS ||
                vkCreateSemaphore(m_device->getDevice(), &semaphoreInfo, nullptr, &m_frameContext[i].renderFinished) != VK_SUCCESS ||
                vkCreateFence(m_device->getDevice(), &fenceInfo, nullptr, &m_frameContext[i].fence) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
}

void Renderer::recreateSwapChain() {
    // Wait to GPU to finish drawing and then we recreate the SwapChain
    vkDeviceWaitIdle(m_device->getDevice());

    for (auto framebuffer : m_renderTarget.framebuffers) {
        vkDestroyFramebuffer(m_device->getDevice(), framebuffer, nullptr);
    }

    u32 width = m_window->GetWidth();
    u32 height = m_window->GetHeight();
    VkExtent2D windowExtent = {width, height};

    m_swapChain->recreate(windowExtent);
    
    createRenderTarget();

    m_trianglePass->resize(m_renderTarget);
}

VkResult Renderer::acquireNextImage(VkSemaphore signalSemaphore, uint32_t* index) {
    return m_swapChain->acquireNextImage(signalSemaphore, index);
}
VkResult Renderer::present(uint32_t imageIndex, VkSemaphore waitSemaphore) {
    return m_swapChain->present(imageIndex, waitSemaphore);
}