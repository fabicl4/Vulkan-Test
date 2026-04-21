#pragma once

#include <stdexcept>

#include <platform/ISurface.h>

struct GLFWwindow;

namespace platform {

class GLFWSurface final : public ISurface {
public:
    explicit GLFWSurface(GLFWwindow* window) : m_window(window) {}

    ~GLFWSurface();

    VkSurfaceKHR createVulkanSurface(VkInstance instance) const override;

    void getFramebufferSize(int& width, int& height) const override;

private:
    GLFWwindow* m_window; // no owning
};

} // namespace platform