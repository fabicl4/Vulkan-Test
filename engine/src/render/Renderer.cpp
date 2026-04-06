#include <render/Renderer.h>

#include <vulkan/vulkan.h>

#include <platform/window/Window.h>

#include "vulkan/Device.h"
#include "vulkan/SwapChain.h"
#include "vulkan/Surface.h"
#include "vulkan/Instance.h"

Renderer::Renderer(
    Window* window,
    ResourceSystem& resourceSystem
) : 
    m_window(window), 
    m_resourceSystem(resourceSystem)
{
    createVkContext();
}

bool Renderer::initialize()
{   
    // manage fail states...

    createRenderPass();
    createFramebuffers();
    createCommandBuffers();
    createSyncObjects();

    for(auto& sub : m_subRenderers) {
        if (!sub->initialize(m_vkContext.device, m_renderPass)) {
            LOG_ERROR("Failed to initialize subrenderer");
            return false;
        }
    }

    return true;
}

void Renderer::cleanup() {
    vkDeviceWaitIdle(m_vkContext.device->getDevice());
    // ...

    for(auto& sub : m_subRenderers) {
        sub->cleanup(m_vkContext.device);
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(m_vkContext.device->getDevice(), m_frameContext[i].imageAvailable, nullptr);
        vkDestroySemaphore(m_vkContext.device->getDevice(), m_frameContext[i].renderFinished, nullptr);
        vkDestroyFence(m_vkContext.device->getDevice(), m_frameContext[i].fence, nullptr);
    }

    // release pipeline
    //m_pipeline.release();

    // release framebuffers
    for (auto framebuffer : m_framebuffers) {
        vkDestroyFramebuffer(m_vkContext.device->getDevice(), framebuffer, nullptr);
    }

    // release render pass
    vkDestroyRenderPass(
        m_vkContext.device->getDevice(), 
        m_renderPass, nullptr);

    // ...

    m_vkContext.swapChain->cleanup();
    m_vkContext.device->cleanup();
    m_vkContext.surface->destroySurface(m_vkContext.instance->handle());
    m_vkContext.instance->cleanup();
}

void Renderer::beginFrame()
{
    // wait fence
    vkWaitForFences( //TODO: Encapsulate
        m_vkContext.device->getDevice(), 
        1, 
        &m_frameContext[m_currentFrame].fence,
        VK_TRUE, 
        UINT64_MAX
    );

    // acquire image
    VkResult result = acquireNextImage(m_frameContext[m_currentFrame].imageAvailable, &m_imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // reset fences
    vkResetFences(m_vkContext.device->getDevice(), 1,
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

void Renderer::draw()
{
    auto& frame = m_frameContext[m_currentFrame];
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
        
        for (auto& sub : m_subRenderers) {
            //sub->recordCommands(commandBuffer, m_imageIndex);
        }

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
void Renderer::endFrame() {
    // end command buffer ...
    //vkEndCommandBuffer(m_frameContext[m_currentFrame].cmd);

    // submit
    if (m_vkContext.device->submitCommandBuffer(
        m_frameContext[m_currentFrame].cmd,
        m_frameContext[m_currentFrame].imageAvailable,
        m_frameContext[m_currentFrame].renderFinished,
        m_frameContext[m_currentFrame].fence
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

void Renderer::createVkContext() {
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

    m_vkContext.instance = new Instance(enableValidationLayers);
    if (!m_vkContext.instance->initialize()) {
        throw std::runtime_error("failed to create Vulkan instance!");
    }
    
    m_vkContext.surface = new Surface();
    if (!m_vkContext.surface->createSurface(
        m_vkContext.instance->handle(), m_window->getNativeHandle())) {
        throw std::runtime_error("failed to create window surface!");
    }

    m_vkContext.device = new Device(
        m_vkContext.instance->handle(), 
        m_vkContext.surface->getSurface()
    );
    if (!m_vkContext.device->initialize()) {
        throw std::runtime_error("failed to create logical device!");
    }

    m_vkContext.swapChain = new SwapChain(
        m_vkContext.device, 
        m_vkContext.surface->getSurface(), 
        m_window->getExtent()
    );

    if (!m_vkContext.swapChain->initialize()) {
        throw std::runtime_error("failed to create swap chain!");
    }
}

#pragma region EventHandlers

void Renderer::onWindowResize(int width, int height) {
    recreateSwapChain();
    // ...
}

#pragma endregion

//-----------------------------------------------------------------------------
void Renderer::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_vkContext.swapChain->getSwapChainImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_vkContext.device->getDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

// Create front and back framebuffers
void Renderer::createFramebuffers() {
    m_framebuffers.clear();

    uint32_t imageCount = static_cast<uint32_t>(m_vkContext.swapChain->imageCount());
    VkExtent2D extent = m_vkContext.swapChain->getSwapChainExtent();

    m_framebuffers.resize(imageCount);

    for (uint32_t i = 0; i < imageCount; i++) {
        VkImageView imageView = m_vkContext.swapChain->getImageView(i);

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;      // Render pass ya creado
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &imageView;
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_vkContext.device->getDevice(), &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Fallo al crear framebuffer");
        }
    }
}

void Renderer::createCommandBuffers() {
    VkCommandPool commandPool = m_vkContext.device->getCommandPool();

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; 
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    // 3. Crear un array con los handles de los command buffers
    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> cmdBuffers;
    if (vkAllocateCommandBuffers(m_vkContext.device->getDevice(), &allocInfo, cmdBuffers.data()) != VK_SUCCESS) {
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
            if (vkCreateSemaphore(m_vkContext.device->getDevice(), &semaphoreInfo, nullptr, &m_frameContext[i].imageAvailable) != VK_SUCCESS ||
                vkCreateSemaphore(m_vkContext.device->getDevice(), &semaphoreInfo, nullptr, &m_frameContext[i].renderFinished) != VK_SUCCESS ||
                vkCreateFence(m_vkContext.device->getDevice(), &fenceInfo, nullptr, &m_frameContext[i].fence) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
}

void Renderer::recreateSwapChain() {
    // Wait to GPU to finish drawing and then we recreate the SwapChain
    vkDeviceWaitIdle(m_vkContext.device->getDevice());

    // 1. destroy front and back frame buffers
    for (auto framebuffer : m_framebuffers) {
        vkDestroyFramebuffer(m_vkContext.device->getDevice(), framebuffer, nullptr);
    }
    // 2. cleanup swapchain
    // 3. recreate swapchain
    m_vkContext.swapChain->recreate(m_window->getExtent());
    // 4. recreate front and back framebuffers
    createFramebuffers();
}

VkResult Renderer::acquireNextImage(VkSemaphore signalSemaphore, uint32_t* index) {
    return m_vkContext.swapChain->acquireNextImage(signalSemaphore, index);
}
VkResult Renderer::present(uint32_t imageIndex, VkSemaphore waitSemaphore) {
    return m_vkContext.swapChain->present(imageIndex, waitSemaphore);
}
