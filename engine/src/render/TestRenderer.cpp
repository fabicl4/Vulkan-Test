#include "TestRenderer.h"

#include "vulkan/Device.h"

bool TestRenderer::initialize(Device* device, VkRenderPass renderPass) {
    createVertexBuffer(device);
    createIndexBuffer(device);

    // pipeline

    return true;
}

void TestRenderer::recordCommands(VkCommandBuffer commandBuffer, uint32_t imageIndex) {

    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

void TestRenderer::cleanup(Device* device) {
    vkDestroyBuffer(device->getDevice(), indexBuffer, nullptr);
    vkFreeMemory(device->getDevice(), indexBufferMemory, nullptr);

    vkDestroyBuffer(device->getDevice(), vertexBuffer, nullptr);
    vkFreeMemory(device->getDevice(), vertexBufferMemory, nullptr);
}

void TestRenderer::createVertexBuffer(Device* device) {
    
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(device->getDevice(), stagingBufferMemory);

    device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    device->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(device->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device->getDevice(), stagingBufferMemory, nullptr);

}
void TestRenderer::createIndexBuffer(Device* device) {
    

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(device->getDevice(), stagingBufferMemory);

    device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    device->copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(device->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device->getDevice(), stagingBufferMemory, nullptr);
}