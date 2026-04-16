#include "ShaderLoader.h"

Shader ShaderLoader::load(const std::string& filepath, VkShaderStageFlagBits stage) {
    auto code = readFile(filepath);
};