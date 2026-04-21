#include <renderer/types/Vertex.h>

namespace renderer {

VkFormat VertexLayout::toVkFormat(VertexAttributeFormat format) const
{
    switch (format)
    {
        case VertexAttributeFormat::FLOAT1:  return VK_FORMAT_R32_SFLOAT;
        case VertexAttributeFormat::FLOAT2:  return VK_FORMAT_R32G32_SFLOAT;
        case VertexAttributeFormat::FLOAT3:  return VK_FORMAT_R32G32B32_SFLOAT;
        case VertexAttributeFormat::FLOAT4:  return VK_FORMAT_R32G32B32A32_SFLOAT;

        case VertexAttributeFormat::INT1:    return VK_FORMAT_R32_SINT;
        case VertexAttributeFormat::INT2:    return VK_FORMAT_R32G32_SINT;
        case VertexAttributeFormat::INT3:    return VK_FORMAT_R32G32B32_SINT;
        case VertexAttributeFormat::INT4:    return VK_FORMAT_R32G32B32A32_SINT;

        case VertexAttributeFormat::UINT1:   return VK_FORMAT_R32_UINT;
        case VertexAttributeFormat::UINT2:   return VK_FORMAT_R32G32_UINT;
        case VertexAttributeFormat::UINT3:   return VK_FORMAT_R32G32B32_UINT;
        case VertexAttributeFormat::UINT4:   return VK_FORMAT_R32G32B32A32_UINT;
        default:
            LOG_ERROR("Unsupported vertex format");
    }

    return VK_FORMAT_UNDEFINED;
}

uint32_t VertexLayout::formatSize(VertexAttributeFormat format) {
    switch (format)
    {
        case VertexAttributeFormat::FLOAT1:  return sizeof(float);
        case VertexAttributeFormat::FLOAT2:  return 2 * sizeof(float);
        case VertexAttributeFormat::FLOAT3:  return 3 * sizeof(float);
        case VertexAttributeFormat::FLOAT4:  return 4 * sizeof(float);

        case VertexAttributeFormat::INT1:    return sizeof(int);
        case VertexAttributeFormat::INT2:    return 2 * sizeof(int);
        case VertexAttributeFormat::INT3:    return 3 * sizeof(int);
        case VertexAttributeFormat::INT4:    return 4 * sizeof(int);

        case VertexAttributeFormat::UINT1:   return sizeof(unsigned int);
        case VertexAttributeFormat::UINT2:   return 2 * sizeof(unsigned int);
        case VertexAttributeFormat::UINT3:   return 3 * sizeof(unsigned int);
        case VertexAttributeFormat::UINT4:   return 4 * sizeof(unsigned int);
        default:
            LOG_ERROR("Unsupported vertex format");
    }

    return 0;
}

uint32_t VertexLayout::componentCount(VertexAttributeFormat format) 
{
    return 0;
}

}; // namespace renderer::pipeline