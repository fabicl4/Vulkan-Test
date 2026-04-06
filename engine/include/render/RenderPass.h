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

    RenderPass(Device& device, const Config& config);
    ~RenderPass();

    bool initialize(SwapChain& swapChain);
    void cleanup();

    void addSubRenderer(SubRenderer* subRenderer);
    void removeSubRenderer(SubRenderer* subRenderer);

    void beginFrame();
    void endFrame();

    void recordCommands(VkCommandBuffer commandBuffer, uint32_t imageIndex, VkFramebuffer framebuffer, const VkExtent2D& extent);

    VkRenderPass getVkRenderPass() const { return m_renderPass; }

private:
    bool createRenderPass();
    bool createFramebuffers(SwapChain& swapChain);
    void destroyFramebuffers();

private:
    Device& m_device;
    Config m_config;

    VkRenderPass m_renderPass = VK_NULL_HANDLE;

    std::vector<VkFramebuffer> m_framebuffers;
    std::vector<SubRenderer*> m_subRenderers;
};

