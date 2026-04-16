#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include <core/types.h>
#include <core/Log.h>

#include <render/resources/Pipeline.h>
#include <render/resources/Texture.h>
#include <render/resources/Shader.h>

#include <render/vulkan/Context.h>

#include <render/RenderContext.h>
#include <render/FrameContext.h>

#include "RenderTarget.h"

class SubRenderer;
class SwapChain;

struct AttachmentDesc {
    VkFormat            format      = VK_FORMAT_UNDEFINED;
    VkAttachmentLoadOp  loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
    VkAttachmentStoreOp storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
    VkImageLayout       initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout       finalLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    bool                isDepth     = false;
};

struct RenderPassDesc {
    std::string name; // para debugging y render graph

    std::vector<AttachmentDesc> colorAttachments;
    AttachmentDesc              depthAttachment;
    bool                        hasDepth = false;

    VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
};

// RenderPass is a small wrapper that owns a Vulkan render pass, its framebuffers,
// and a set of subrenderers that record draw work inside it.
//
// This keeps the frame orchestration outside of the concrete renderers and
// makes it easier to evolve later into a RenderGraph node.
class RenderPass {
public:
    explicit RenderPass(Context ctx) : m_ctx(ctx) {};
    virtual ~RenderPass() = default;

    // Functionality:
    // 1. create render pass.
    // 2. create its own resources
    // 3. create framebuffers
    virtual bool initialize(RenderTarget& renderTarget);

    // Cleanup resources created by the render pass.
    virtual void cleanup();

    // Prepare data for rendering. Called every frame before recording commands.
    virtual void prepare(const FrameContext& frameContext) {};

    // parameters are the render context for the current frame, provided by the renderer.
    void execute(const FrameContext& frameContext) {
        
        beginRenderPass(frameContext.cmd, frameContext.imageIndex, frameContext.extent);

            doRecordCommands(frameContext.cmd, frameContext.imageIndex, frameContext.frameIndex, m_framebuffers[frameContext.imageIndex], frameContext.extent);

        endRenderPass(frameContext.cmd);
    }

    // Called when the swapchain is resized (recreate framebuffers)
    void resize(RenderTarget& renderTarget);

    // El RenderTarget de salida de este pass (para que el siguiente pass lo lea).
    // Solo válido después de initialize().
    const RenderTarget& getOutputTarget() const { return m_outputTarget; }

    //const Descriptor& getDescriptor() const { return m_desc; }
    VkRenderPass getVkRenderPass() const { return m_renderPass; }
    
private:
    void beginRenderPass(VkCommandBuffer commandBuffer, uint32_t imageIndex, const VkExtent2D& extent);
    void endRenderPass(VkCommandBuffer commandBuffer);
    
    virtual void doRecordCommands(
        VkCommandBuffer commandBuffer, 
        uint32_t imageIndex, 
        uint32_t frameIndex,
        VkFramebuffer framebuffer, 
        const VkExtent2D& extent
    ) = 0;

private:
    bool createRenderPass();
    bool createFramebuffers(const RenderTarget& target);
    void destroyFramebuffers();

protected:

    Context& m_ctx;

    VkRenderPass m_renderPass = VK_NULL_HANDLE;

    std::vector<VkFramebuffer> m_framebuffers;

    // El target que este pass produjo (las image views de sus attachments).
    // Los passes siguientes pueden leerlo como TextureHandles.
    RenderTarget m_outputTarget;
};

