#pragma once

#include <vulkan/vulkan.h>
#include <core/types.h>

#include "vulkan/Context.h"

#include "Pipeline.h"
#include "Shader.h"

// NOTE: I use as a template the following link
// https://github.com/blurrypiano/littleVulkanEngine/blob/main/src/lve_pipeline.hpp


// Struct pipeline object: 
//------------------------
// Vertex shander
// Fragment shader
// Rasterization
// Blend modes
// Depth-Stencil
// Dynamic state
// Layout

struct PipelineConfigInfo {
    // Shaders
    VkPipelineShaderStageCreateInfo vertShaderStageInfo;
    VkPipelineShaderStageCreateInfo fragShaderStageInfo;

    // Vertex Input layout
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;

    // Input Assembly Stage
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;

    // Rasterization Stage
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;

    // 
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    
    // Color Blending Stage
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;

    // Depth-Stencil Stage
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

    // Vertex descriptor storage
    std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;

    // Dynamic State
    VkPipelineViewportStateCreateInfo viewportInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;

    VkRenderPass renderPass = nullptr;

    uint32_t subpass = 0;
};

struct VertexInputDescription { // Optional
    std::vector<VkVertexInputBindingDescription> bindings{};
    std::vector<VkVertexInputAttributeDescription> attributes{};
};

struct DescriptorBinding {
    uint32_t           binding;
    VkDescriptorType   type;   // UNIFORM_BUFFER, COMBINED_IMAGE_SAMPLER, etc.
    uint32_t           count = 1;
    VkShaderStageFlags stages;
};

// push constant Range descriptor.
struct PushConstantRange {
    VkShaderStageFlags stages;
    uint32_t           offset = 0;
    uint32_t           size;
};

// Builder pattern
// NOTE: Should i use a data approach instead of a builder? Maybe a struct with all the config and a single build function that takes it as input?
class PipelineBuilder {
public:
    explicit PipelineBuilder(Context& ctx) : m_ctx(ctx) 
    {
        m_config = defaultPipelineConfigInfo();
    }

    PipelineBuilder& setShaders(const Shader& vert, const Shader& frag);
    PipelineBuilder& setVertexLayout(const VertexInputDescription&);
    PipelineBuilder& setTopology(VkPrimitiveTopology);
    PipelineBuilder& setDepthTest(bool enable, bool write);
    PipelineBuilder& setBlending(bool enable);
    PipelineBuilder& setCullMode(
        VkCullModeFlags cull,
        VkFrontFace front = VK_FRONT_FACE_COUNTER_CLOCKWISE
    );
    PipelineBuilder& setRenderPass(VkRenderPass, uint32_t subpass = 0);

    PipelineBuilder& addDescriptorSet(std::vector<DescriptorBinding> bindings);
    PipelineBuilder& addPushConstant(VkShaderStageFlags stages, uint32_t size,
                                     uint32_t offset = 0);

    Pipeline build();

private:
    Context&       m_ctx;
    PipelineConfigInfo  m_config {};

    PipelineConfigInfo defaultPipelineConfigInfo();

    // Layout descriptors
    std::vector<std::vector<DescriptorBinding>> m_descriptorSets;
    std::vector<PushConstantRange>              m_pushConstants;

};