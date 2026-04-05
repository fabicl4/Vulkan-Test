#pragma once

#include <vulkan/vulkan.h>

#include <platform/window/Window.h>

class Surface
{
public:
    Surface() = default;
    ~Surface() = default;

    // create surface
    bool createSurface(VkInstance instance, Window& window);

    // destroy surface
    void destroySurface(VkInstance instance);

    VkSurfaceKHR getSurface() const { return m_surface; }

private:
    VkSurfaceKHR m_surface;
};