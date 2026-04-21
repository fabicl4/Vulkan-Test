#pragma once

// base for Render pipeline

#include <vector>
#include <memory>
#include "RenderPass.h"
#include "RenderTarget.h"

namespace renderer {

    
class GraphicsDevice;

struct CameraData {
    glm::mat4  view;
    glm::mat4  proj;
    glm::mat4  viewProj;
    glm::vec3  position;
    float      nearPlane;
    float      farPlane;
};

struct LightData {
    glm::vec3  direction;
    glm::vec3  color;
    float      intensity;
};

/*
struct RenderingData {
    CameraData          camera;
    LightData           mainLight;
    const DrawList*     drawList    = nullptr;
    const FrameContext* frameCtx    = nullptr;   // cmd, imageIndex, frameIndex
    VkExtent2D          renderExtent;
};
*/

class RenderPipeline {
public:
    RenderPipeline(Device& device, SwapChain& swapChain);
    ~RenderPipeline() = default;

    void addPass(std::unique_ptr<RenderPass> pass);

    // Crea todos los RenderTargets intermedios y llama initialize() en cada pass.
    // Debe llamarse después de todos los addPass().
    bool build();

    // Ejecuta todos los passes en orden para el frame actual.
    void execute(VkCommandBuffer cmd, uint32_t imageIndex);

    // Reconstruye todo cuando el swapchain cambia de tamaño.
    bool resize(uint32_t width, uint32_t height);

    void cleanup();

private:
    // Crea un RenderTarget offscreen para un pass intermedio
    // (no el swapchain). Los attachments se crean según el Descriptor del pass.
    bool createIntermediateTarget(RenderPass& pass, uint32_t imageCount);

    Device&    m_device;
    SwapChain& m_swapChain;

    std::vector<std::unique_ptr<RenderPass>> m_passes;

    // El target final: el swapchain.
    RenderTarget m_swapChainTarget;
};

}; // namespace renderer::pipeline
