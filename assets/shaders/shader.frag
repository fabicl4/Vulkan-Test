#version 450

// https://vulkan-tutorial.com/code/18_shader_vertexbuffer.frag

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}