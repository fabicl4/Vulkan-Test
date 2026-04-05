#version 450

layout(set = 0, binding = 0) uniform Camera {
    mat4 view_proj;
} camara_ubo;

layout(set = 1, binding = 0) uniform SpriteSheet {
    ivec2 image_size;
    ivec2 margins;
    ivec2 offset;
    ivec2 spriteSize;
} spritesheet_ubo;

struct SpriteInstance {
    mat4 model;
    vec2 texPos;
    vec4 color;
};

layout(std430, set = 1, binding = 0) buffer InstanceBuffer {
    SpriteInstance instances[];
};

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUVs;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragUV;

vec2 calculate_uvs(SpriteInstance instance) {
    int x = instance.texPos.x;
    int y = instance.texPos.y;

    vec2 pixelPos = spritesheet_ubo.offset +
                    vec2(x, y) *
                    (spritesheet_ubo.spriteSize + spritesheet_ubo.margins);
    
    vec2 baseUV = pixelPos / spritesheet_ubo.image_size;
    vec2 sizeUV = spritesheet_ubo.spriteSize / spritesheet_ubo.image_size;

    return baseUV + inUVs * sizeUV;
}

void main() {
    SpriteInstance instance = instances[gl_InstanceIndex];

    vec4 worldPos = instance.model * vec4(inPosition, 0.0, 1.0);
    gl_Position = camara_ubo.view_proj * worldPos;
    
    fragColor = instance.color;

    fragUV = calculate_uvs()
    
}