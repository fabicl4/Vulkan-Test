#include "Device.h"

bool Device::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        LOG_CRITICAL("[Device]failed to find GPUs with Vulkan support!");
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (const auto& m_device : devices) {
        if (isDeviceSuitable(m_device)) {
            m_physicalDevice = m_device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
        LOG_CRITICAL("[Device]failed to find a suitable GPU!");
        return false;
    }

    return true;
}

bool Device::isDeviceSuitable(VkPhysicalDevice m_device) {
    QueueFamilyIndices indices = findQueueFamilies(m_device);

    bool extensionsSupported = checkDeviceExtensionSupport(m_device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice m_device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(m_device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(m_device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice m_device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(m_device, i, m_surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

bool Device::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
        LOG_CRITICAL("[Device]failed to create logical m_device!");
        return false;
    }

    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);

    return true;
}

SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice m_device) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device, m_surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_device, m_surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_device, m_surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_device, m_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_device, m_surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

void Device::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool);
}

VkCommandBuffer Device::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void Device::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphicsQueue);

    vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}

//-----------------------------------------------------------------------------
// Buffer
//-----------------------------------------------------------------------------
#pragma region Buffer

VkBufferUsageFlags toVkUsage(BufferUsage usage)
{
    VkBufferUsageFlags flags = 0;
    auto u = static_cast<unsigned int>(usage);

    if (u & static_cast<unsigned int>(BufferUsage::Vertex))
        flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    if (u & static_cast<unsigned int>(BufferUsage::Index))
        flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    if (u & static_cast<unsigned int>(BufferUsage::Uniform))
        flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    if (u & static_cast<unsigned int>(BufferUsage::Storage))
        flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    if (u & static_cast<unsigned int>(BufferUsage::CopySrc))
        flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    if (u & static_cast<unsigned int>(BufferUsage::CopyDst))
        flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    if (u & static_cast<unsigned int>(BufferUsage::Indirect))
        flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

    if (u & static_cast<unsigned int>(BufferUsage::UniformTexel))
        flags |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;

    if (u & static_cast<unsigned int>(BufferUsage::StorageTexel))
        flags |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;

    return flags;
}

VkMemoryPropertyFlags toVkMemoryFlags(MemoryType type)
{
    switch (type)
    {
        case MemoryType::GPU_ONLY:
            return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        case MemoryType::CPU_ONLY:
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_CACHED_BIT;

        case MemoryType::CPU_TO_GPU:
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        case MemoryType::GPU_TO_CPU:
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    }

    return 0;
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    return UINT32_MAX; // no encontrado
}

bool Device::createBuffer(const BufferDescriptor& desc,  Buffer& buffer) {
    // NOTE:
    // ---- 
    //  - If memType is GPU_ONLY then VK_BUFFER_USAGE_TRANSFER_DST_BIT
    //  - If it is a staging buffer VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    //  - Some buffer needs to be aligned
    //  - Use VMA (Vulkan Memory Allocator) instead of allocation the buffer every time.
    if (desc.size == 0) {
        LOG_ERROR("[Device]Could NOT allocate a buffer of size 0!");
        return false;
    }

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.size = desc.size;
    bufferInfo.usage = toVkUsage(desc.usage);
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device, &bufferInfo, NULL, &buffer.handle) != VK_SUCCESS) {
        LOG_ERROR("[Device]Could NOT create the buffer!");
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, buffer.handle, &memRequirements);

    VkMemoryPropertyFlags memFlags = toVkMemoryFlags(desc.memType);

    uint32_t memoryTypeIndex = findMemoryType(
        memRequirements.memoryTypeBits,
        memFlags
    );

    if (memoryTypeIndex == UINT32_MAX) {
        LOG_ERROR("");
        return false;
    }

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &buffer.memory) != VK_SUCCESS) {
        LOG_ERROR("[Device]Failed to allocate vertex buffer memory!");
        return false;
    }

    if (vkBindBufferMemory(m_device, buffer.handle, buffer.memory, 0) != VK_SUCCESS) {
        LOG_ERROR("");
        return false;
    }

    // buffer metadata
    buffer.usage   = desc.usage;
    buffer.memType = desc.memType;
    buffer.size    = desc.size;

    return true;
}

void Device::destroyBuffer(Buffer& buffer) {
    if (buffer.handle)
        vkDestroyBuffer(m_device, buffer.handle, nullptr);

    if (buffer.memory)
        vkFreeMemory(m_device, buffer.memory, nullptr);

    buffer = {};
}

/*
void Device::uploadData(Buffer& buffer, const void* data, VkDeviceSize size) {
    // TODO: change this
    if (size > buffer.size) {
        LOG_ERROR("[Device] uploadData: size exceeds buffer capacity");
        return;
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    VkBufferCreateInfo stagingInfo{};
    stagingInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingInfo.size = size;
    stagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device, &stagingInfo, nullptr, &stagingBuffer) != VK_SUCCESS) {
        LOG_CRITICAL("[Device] Failed to create staging buffer");
        return;
    }

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(m_device, stagingBuffer, &memReqs);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &stagingMemory) != VK_SUCCESS) {
        vkDestroyBuffer(m_device, stagingBuffer, nullptr);
        LOG_CRITICAL("[Device] Failed to allocate staging memory");
        return;
    }

    vkBindBufferMemory(m_device, stagingBuffer, stagingMemory, 0);

    void* mappedData = nullptr;
    vkMapMemory(m_device, stagingMemory, 0, size, 0, &mappedData);
    memcpy(mappedData, data, static_cast<size_t>(size));
    vkUnmapMemory(m_device, stagingMemory);

    VkCommandBufferAllocateInfo cmdAlloc{};
    cmdAlloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAlloc.commandPool = m_commandPool;
    cmdAlloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdAlloc.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    vkAllocateCommandBuffers(m_device, &cmdAlloc, &cmdBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmdBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(cmdBuffer, stagingBuffer, buffer.handle, 1, &copyRegion);

    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphicsQueue);

    vkFreeCommandBuffers(m_device, m_commandPool, 1, &cmdBuffer);

    vkDestroyBuffer(m_device, stagingBuffer, nullptr);
    vkFreeMemory(m_device, stagingMemory, nullptr);
}

void Device::uploadData(Buffer& dstBuffer, const void* data, VkDeviceSize size) {
    // Creates an stagging buffer

    // Map memory

    // Copy memory to dstBuffer
}*/

void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}


bool Device::createVertexBuffer(const std::vector<float>& vertices, Buffer& vertexBuffer) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    // Create stagging buffer
    BufferDescriptor stagingBufferDesc {};
    stagingBufferDesc.memType = MemoryType::CPU_TO_GPU;
    stagingBufferDesc.size = bufferSize;
    stagingBufferDesc.usage = BufferUsage::CopySrc;

    Buffer stagingBuffer;

    if (!createBuffer(stagingBufferDesc, stagingBuffer)) {
        return false;
    }

    // Map memory
    void* data;
    vkMapMemory(m_device, stagingBuffer.memory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(m_device, stagingBuffer.memory);

    // Create the vertex buffer
    BufferDescriptor vertexBufferDesc {};
    vertexBufferDesc.memType = MemoryType::GPU_ONLY;
    vertexBufferDesc.size = bufferSize;
    vertexBufferDesc.usage = BufferUsage::Vertex | BufferUsage::CopyDst;

    if (!createBuffer(vertexBufferDesc, vertexBuffer)) {
        return false;
    }

    // Copy data
    copyBuffer(stagingBuffer.handle, vertexBuffer.handle, bufferSize);

    // Destroy the staging buffer
    destroyBuffer(stagingBuffer);

    return true;
}

#pragma endregion Buffer

#pragma region Shader
VkShaderModule Device::createShaderModule(const std::vector<char>& code) {
    VkShaderModule shaderModule;

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }
    
    return shaderModule;
}

void Device::destroyShaderModule(VkShaderModule shaderModule) {
    if (shaderModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(m_device, shaderModule, nullptr);
    }
}

#pragma endregion Shader

void Device::createGraphicsPipeline(VkShaderModule vertShader, VkShaderModule fragShader,
    const PipelineConfigInfo& configInfo, Pipeline& pipeline)
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShader;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShader;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    // We are not supplying any data
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(
        configInfo.attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = configInfo.attributeDescriptions.data();
    vertexInputInfo.pVertexBindingDescriptions = configInfo.bindingDescriptions.data();

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
    pipelineInfo.pViewportState = &configInfo.viewportInfo;
    pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
    pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
    pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
    pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
    pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;

    pipelineInfo.layout = configInfo.pipelineLayout;
    pipelineInfo.renderPass = configInfo.renderPass;
    pipelineInfo.subpass = configInfo.subpass;

    pipelineInfo.basePipelineIndex = -1;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(
            m_device,
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            nullptr,
            &pipeline.handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline");
    }
}

void Device::destroyPipeline(Pipeline& pipeline){
    vkDestroyPipeline(m_device, pipeline.handle, nullptr);
    vkDestroyPipelineLayout(m_device, pipeline.layout, nullptr);

    pipeline.handle = VK_NULL_HANDLE;
    pipeline.layout = VK_NULL_HANDLE;
}

#pragma region Image
bool Device::createTextureImage(int width, int height, 
    int numChannels, unsigned char* pixels, 
    VkImage& image, VkDeviceMemory& imageMemory) 
{
    // Create a staging buffer to upload texture data
    BufferDescriptor stagingBufferDesc {};
    Buffer stagingBuffer {};

    VkDeviceSize imageSize = width * height * numChannels;

    stagingBufferDesc.size = imageSize;
    stagingBufferDesc.memType = MemoryType::CPU_TO_GPU;
    stagingBufferDesc.usage = BufferUsage::CopySrc;

    if (!createBuffer(stagingBufferDesc, stagingBuffer)) {
        LOG_ERROR("Failed to create the staging buffer for the image");
        return false;
    }

    // Upload data
    void* data;
    vkMapMemory(m_device, stagingBuffer.memory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(m_device, stagingBuffer.memory);

    VkDeviceMemory textureImageMemory;

    VkImageType imageType = VK_IMAGE_TYPE_2D;
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    uint32_t mipLevels = 1;
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    createImage(width, height, format, tiling, usage, properties, 
        image, imageMemory);
    
    return false;
}


void Device::createImage(uint32_t width, uint32_t height, 
    VkFormat format, 
    VkImageTiling tiling, 
    VkImageUsageFlags usage, 
    VkMemoryPropertyFlags properties, 
    VkImage& image, VkDeviceMemory& imageMemory
) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(m_device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(m_device, image, imageMemory, 0);
}

#pragma endregion Image