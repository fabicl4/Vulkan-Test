#include <resources/Texture.h>

#include <render/vulkan/Buffer.h>

#include <render/vulkan/internal/Device.h>

#include <stb_image.h>

bool TextureManager::load(const std::string& name, 
    const std::string& filepath) 
{
    int width, height, numChannels;

    unsigned char* data =  stbi_load(filepath.c_str(), &width, &height, &numChannels, 
        STBI_rgb_alpha);

    if (!data) {
        LOG_ERROR("Could NOT load the texture {}, filepath: {}", name, filepath);
        return false;
    } 
    
    // once loaded, we create the texture 
    // TODO: Create the texture

    stbi_image_free(data);


    return true;
}