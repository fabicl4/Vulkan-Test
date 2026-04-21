#pragma once

#include <vulkan/vulkan.h>

//#include "vulkan/Context.h"

#include "pipeline/RenderTarget.h"

namespace renderer {

class SwapChain;

struct RenderContext {
//    Context*        ctx; // Does not own the context, just a reference for convenience
    SwapChain*      swapChain; // same as context
    
    // swapchain render target (final output)
    RenderTarget    swapChainTarget; // swapchain->asRenderTarget();
    
    // Helpers de conveniencia para no tener que ir al swapchain directamente
    VkExtent2D extent()      const { return swapChainTarget.extent; }
    VkFormat   colorFormat() const { return swapChainTarget.colorAttachments[0].format; }
    uint32_t   imageCount()  const { return swapChainTarget.colorCount(); }
};


}; // namespace renderer