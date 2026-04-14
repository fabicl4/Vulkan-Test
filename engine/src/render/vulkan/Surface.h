#pragma once

#include "vkCore.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Surface
{
public:
    Surface() = default;
    ~Surface() = default;

    // create surface
    bool createSurface(VkInstance instance, GLFWwindow* window);

    // destroy surface
    void destroySurface(VkInstance instance);

    VkSurfaceKHR getSurface() const { return m_surface; }

private:
    VkSurfaceKHR m_surface;
};