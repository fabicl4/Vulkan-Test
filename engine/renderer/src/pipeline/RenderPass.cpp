#include <render/RenderPass.h>

#include "render/vulkan/internal/Device.h"
#include "render/vulkan/internal/SwapChain.h"

RenderPass::RenderPass(Device& device, const Config& config) 
    : m_device(device), m_config(config) 
{
    if (!createRenderPass()) {
        throw std::runtime_error("failed to create render pass!");
    }
}

bool RenderPass::initialize(RenderTarget& renderTarget) {
    
    // create framebuffers
    createFramebuffers(renderTarget);

    return true;
}

void RenderPass::resize(RenderTarget& renderTarget) {
    destroyFramebuffers();
    createFramebuffers(renderTarget);
}

void RenderPass::cleanup() {
    destroyFramebuffers();

    vkDestroyRenderPass(m_device.getDevice(), m_renderPass, nullptr);
}

void RenderPass::beginRenderPass(VkCommandBuffer commandBuffer, uint32_t imageIndex, 
    const VkExtent2D& extent) 
{

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = m_framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::endRenderPass(VkCommandBuffer commandBuffer) 
{
    vkCmdEndRenderPass(commandBuffer);
}

bool RenderPass::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_config.colorFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = m_config.loadOp;
    colorAttachment.storeOp = m_config.storeOp;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = m_config.initialLayout;
    colorAttachment.finalLayout = m_config.finalLayout;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = m_config.bindPoint;
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

    if (vkCreateRenderPass(m_device.getDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
        LOG_ERROR("failed to create render pass!");
        return false;
    }

    return true;
}

bool RenderPass::createFramebuffers(const RenderTarget& target) {
    m_framebuffers.resize(target.colorCount());

    for (size_t i = 0; i < target.colorCount(); i++) {
        VkImageView attachments[] = {
            target.colorAttachments[i].imageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = target.extent.width;
        framebufferInfo.height = target.extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_device.getDevice(), &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS) {
            LOG_ERROR("failed to create framebuffer!");
            return false;
        }
    }

    return true;
}

void RenderPass::destroyFramebuffers() {
    for (auto framebuffer : m_framebuffers) {
        vkDestroyFramebuffer(m_device.getDevice(), framebuffer, nullptr);
    }
    m_framebuffers.clear();
}