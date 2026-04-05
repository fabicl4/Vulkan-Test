#version 450

layout(set = 2, binding = 0) uniform sampler2D textures;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 tex = texture(textures, fragUV);

    outColor = tex * fragColor;
}