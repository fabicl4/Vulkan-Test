#pragma once

#include "include.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/types.h>
#include <core/Log.h>

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