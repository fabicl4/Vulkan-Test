// platform/ISurface.h
#pragma once

#include <vulkan/vulkan.h>

namespace platform {

class ISurface {
public:
    virtual ~ISurface() = default;

    // El renderer llama esto para obtener la surface de Vulkan.
    virtual VkSurfaceKHR createVulkanSurface(VkInstance instance) const = 0;

    // Útil para que el renderer pregunte el tamaño del framebuffer.
    virtual void getFramebufferSize(int& width, int& height) const = 0;
};

} // namespace platform