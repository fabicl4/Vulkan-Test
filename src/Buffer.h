#pragma once

#include <vulkan/vulkan.h>

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
        other.debugName = nullptr;
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
};