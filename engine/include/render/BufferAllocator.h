#pragma once

#include "vulkan/Buffer.h"

class Device;

class BufferAllocator {
public:
    BufferAllocator(Device& device);

    bool createVertexBuffer(const float vertices, Buffer& vertexBuffer);
    bool createIndexBuffer(const float vertices, Buffer& indexBuffer);
    //...

private:
    Device& m_device;
    // VMA Allocator

    bool createBuffer(const BufferDescriptor& desc, Buffer& buffer);
};