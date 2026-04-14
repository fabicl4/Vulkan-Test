#include <engine.h>

#include <iostream>

#include "render/passes/TrianglePass.h"
#include "render/RenderTarget.h"

class TriangleApp : public Application {
public:
    TriangleApp(int width, int height, std::string name) : Application(width, height, name) {}

protected:
    bool onCreate() override {
        std::cout << "TriangleApp created!" << std::endl;

        m_renderTarget = m_renderer->getRenderTarget();

        m_trianglePass.initialize(m_renderTarget);
        
        return true;
    }

    bool onUpdate(double ts) override {
        // Update logic here
        return true;
    }

    bool onRender() override {
        // Render logic here
        m_trianglePass.prepare();

        m_renderer->beginFrame();

            VkCommandBuffer cmdBuffer = m_renderer->getCurrentCommandBuffer(); // Get the command buffer for the current frame
            u32 imageIndex = m_renderer->getCurrentImageIndex(); // Get the index of the current swapchain image
            VkFramebuffer framebuffer = m_renderer->getRenderTarget().framebuffers[imageIndex]; //
            VkExtent2D extent = m_renderer->getRenderTarget().extent; // Get the extent of the render target
            
            m_trianglePass.execute(cmdBuffer, imageIndex, extent); // Execute the triangle pass

        m_renderer->endFrame();
        return true;
    }

    bool onDestroy() override {
        std::cout << "TriangleApp destroyed!" << std::endl;
        return true;
    }

private:

    RenderTarget m_renderTarget;

private:
    TrianglePass m_trianglePass {*m_renderer->device(), {}};

    RenderTarget m_renderTarget;
};

int main() {
    TriangleApp app(800, 600, "Hello Triangles");
    app.run();

    return 0;
}