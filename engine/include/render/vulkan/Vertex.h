#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

#include <core/types.h>
#include <core/Log.h>

enum class VertexAttributeFormat {
    FLOAT1, // VK_FORMAT_R32_SFLOAT
    FLOAT2, // VK_FORMAT_R32G32_SFLOAT
    FLOAT3, // VK_FORMAT_R32G32B32_SFLOAT
    FLOAT4, // VK_FORMAT_R32G32B32A32_SFLOAT

    INT1,   // VK_FORMAT_R32_SINT
    INT2,   // VK_FORMAT_R32G32_SINT
    INT3,   // VK_FORMAT_R32G32B32_SINT
    INT4,   // VK_FORMAT_R32G32B32A32_SINT

    UINT1,  // VK_FORMAT_R32_UINT
    UINT2,  // VK_FORMAT_R32G32_UINT
    UINT3,  // VK_FORMAT_R32G32B32_UINT
    UINT4,  // VK_FORMAT_R32G32B32A32_UINT

    // TODO: Add more formats
    
    DOUBLE1, // VK_FORMAT_R64_SFLOAT
    DOUBLE2, // VK_FORMAT_R64G64_SFLOAT
    DOUBLE3, // VK_FORMAT_R64G64B64_SFLOAT
    DOUBLE4, // VK_FORMAT_R64G64B64A64_SFLOAT
};

// NOTE: add u8, u16, etc you'll need alignment logic.

struct VertexAttribute {
    std::string name;       // debug name

    uint32_t    location;
    //uint32_t    binding;    // unit
    VertexAttributeFormat    format;
    uint32_t    offset = 0;

    VertexAttribute(
        const std::string& _name,
        uint32_t _location,
        VertexAttributeFormat _format)
        : name(_name),
          location(_location),
          format(_format),
          offset(0)
    {}
};

/*
 *  TODO:
 *  - Support other types of VkVertexInputRate inputRate
 *  - Sort attributes by location, for now we asume the location match the index
 *  - Allow multiple bindings
 */

class VertexLayout {
public:
    VertexLayout(std::initializer_list<VertexAttribute> attributes, uint32_t binding = 0)
        : m_attributes(attributes), m_binding(binding)
    {
        calculateStrideAndOffsets();
        LOG_TRACE("[VertexLayout] Created vertex layout with stride {} and {} attributes", m_stride, m_attributes.size());
    }

    VkVertexInputBindingDescription getBindingDescription() const noexcept
    {
        VkVertexInputBindingDescription bindingDescription{};

        bindingDescription.binding = m_binding;
        bindingDescription.stride = m_stride;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // TODO: CHANGE THIS LINE

        return bindingDescription;
    }

    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions () const noexcept
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.resize(m_attributes.size());

        for (size_t i = 0; i < m_attributes.size(); i++)
        {
            attributeDescriptions[i].binding = m_binding;
            attributeDescriptions[i].location = m_attributes[i].location;
            attributeDescriptions[i].format = toVkFormat(m_attributes[i].format);
            attributeDescriptions[i].offset = m_attributes[i].offset;
        }

        return attributeDescriptions;
    }

    // getters
    const std::vector<VertexAttribute>& getAttributes() const { return m_attributes; }
    uint32_t getStride() const { return m_stride; }

private:
    void calculateStrideAndOffsets() {
        uint32_t offset = 0;

        for (auto& attr : m_attributes) {
            attr.offset = offset;
            uint32_t size = formatSize(attr.format);
            offset += size;
        }

        m_stride = offset;
    }

    // helpers
    VkFormat toVkFormat(VertexAttributeFormat format) const;

    static uint32_t formatSize(VertexAttributeFormat format);
    static uint32_t componentCount(VertexAttributeFormat format);

private:
    std::vector<VertexAttribute> m_attributes;
    uint32_t m_binding = 0;
    uint32_t m_stride = 0;
};