#include <render/RenderPass.h>

#include "vulkan/Device.h"
#include "vulkan/SwapChain.h"

RenderPass::RenderPass(Device& device, const Config& config) 
    : m_device(device), m_config(config) 
{}

bool RenderPass::initialize(RenderTarget& renderTarget) {
    if (!createRenderPass()) {
        LOG_ERROR("Failed to create render pass");
        return false;
    }

    // create framebuffers
    m_framebuffers = renderTarget.framebuffers;

    return true;
}

void RenderPass::resize(RenderTarget& renderTarget) {
    m_framebuffers = renderTarget.framebuffers;
}

void RenderPass::cleanup() {
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