#pragma once

#include <vulkan/vulkan.h>

#include <string>

namespace renderer {

enum class BufferUsage : unsigned int {
    Vertex  = 1 << 0, // VBO
    Index   = 1 << 1, // EBO
    Uniform = 1 << 2, // UBO
    Storage = 1 << 3, // SSBO
    CopySrc = 1 << 4,
    CopyDst = 1 << 5,
    Indirect = 1 << 6,
    UniformTexel = 1 << 7,
    StorageTexel = 1 << 8
};

// Bitwise operators para BufferUsage
inline BufferUsage operator|(BufferUsage a, BufferUsage b)
{
    return static_cast<BufferUsage>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline BufferUsage operator&(BufferUsage a, BufferUsage b)
{
    return static_cast<BufferUsage>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

enum class MemoryType {
    GPU_ONLY,        // DEVICE_LOCAL
    CPU_ONLY,        // HOST_VISIBLE | HOST_CACHED
    CPU_TO_GPU,      // HOST_VISIBLE | HOST_COHERENT
    GPU_TO_CPU       // HOST_VISIBLE | HOST_CACHED (readback)
};

struct BufferDescriptor {
    BufferUsage usage;
    MemoryType memType;
    VkDeviceSize size; // buffer size on bytes
};

struct Buffer
{
    VkBuffer handle = VK_NULL_HANDLE;; // GPU Handle
    // TODO: Remove this line and code VMA
    VkDeviceMemory memory = VK_NULL_HANDLE; // Buffer allocated memory

    BufferUsage usage{};
    MemoryType memType{};
    // buffer size on bytes
    VkDeviceSize size = 0; 

    std::string debugName;

    // Vulkan use uint32_t as data format
    // OpenGL uses unsigned char
    //unsigned int* data; // pointer to CPU-side data (if applicable)

    // remove copy semantics
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    // move semantics
    Buffer(Buffer&& other) noexcept {
        handle   = other.handle;
        memory   = other.memory;
        usage    = other.usage;
        memType  = other.memType;
        size     = other.size;
        debugName = other.debugName;

        other.handle = VK_NULL_HANDLE;
        other.memory = VK_NULL_HANDLE;
        other.debugName = "";
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other)
        {
            handle   = other.handle;
            memory   = other.memory;
            usage    = other.usage;
            memType  = other.memType;
            size     = other.size;
            debugName = other.debugName;

            other.handle = VK_NULL_HANDLE;
            other.memory = VK_NULL_HANDLE;
            other.debugName = nullptr;
        }
        return *this;
    }

    Buffer() = default;

    void destroy(VkDevice device) {
        if (handle != VK_NULL_HANDLE) {
            vkDestroyBuffer(device, handle, nullptr);
            handle = VK_NULL_HANDLE;
        }
        if (memory != VK_NULL_HANDLE) {
            vkFreeMemory(device, memory, nullptr);
            memory = VK_NULL_HANDLE;
        }
    }
};

inline VkBufferUsageFlags toVkUsage(BufferUsage usage)
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

inline VkMemoryPropertyFlags toVkMemoryFlags(MemoryType type)
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

};