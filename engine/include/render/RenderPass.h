#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include <core/types.h>
#include <core/Log.h>

#include "RenderTarget.h"

class Device;

class SubRenderer;
class SwapChain;

/*
struct Attachment {
    VkFormat format;
    VkAttachmentLoadOp loadOp;
    VkAttachmentStoreOp storeOp;
    VkImageLayout initialLayout;
    VkImageLayout finalLayout;
};

struct Config {
    std::vector<Attachment> colorAttachments;
    std::optional<Attachment> depthAttachment;
};

*/

// RenderPass is a small wrapper that owns a Vulkan render pass, its framebuffers,
// and a set of subrenderers that record draw work inside it.
//
// This keeps the frame orchestration outside of the concrete renderers and
// makes it easier to evolve later into a RenderGraph node.
class RenderPass {
public:
    struct Config {
        VkFormat colorFormat = VK_FORMAT_B8G8R8A8_SRGB;
        VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    };

    RenderPass(Device& device, const Config& config = {});
    virtual ~RenderPass() = default;

    // Functionality:
    // 1. create render pass.
    // 2. create its own resources
    // 3. create framebuffers (or receive them from outside)
    virtual bool initialize(RenderTarget& renderTarget);

    // Prepare data for rendering. Called every frame before recording commands.
    // The render pass can choose to recreate framebuffers or other resources
    // if the render target was resized.
    virtual void prepare() {};
    
    // Cleanup resources created by the render pass.
    virtual void cleanup();

    // Called when the render target is resized.
    virtual void resize(RenderTarget& renderTarget);

    VkRenderPass getVkRenderPass() const { return m_renderPass; }

    void execute(VkCommandBuffer cmd, uint32_t imageIndex, const VkExtent2D& extent) {
        
        beginRenderPass(cmd, imageIndex, extent);

            doRecordCommands(cmd, imageIndex, m_framebuffers[imageIndex], extent);

        endRenderPass(cmd);
    }
    
private:
    void beginRenderPass(VkCommandBuffer commandBuffer, uint32_t imageIndex, const VkExtent2D& extent);
    void endRenderPass(VkCommandBuffer commandBuffer);
    
    virtual void doRecordCommands(VkCommandBuffer commandBuffer, uint32_t imageIndex, VkFramebuffer framebuffer, const VkExtent2D& extent) = 0;

private:
    bool createRenderPass();

protected:
    Device& m_device;
    Config m_config;

    VkRenderPass m_renderPass = VK_NULL_HANDLE;

    std::vector<VkFramebuffer> m_framebuffers;
};

