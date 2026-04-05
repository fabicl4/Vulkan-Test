#pragma once

#include <string>

#include "Resource.h"

#include <core/Log.h>
#include <core/types.h>
#include <render/Texture.h>

class Device;

using TextureHandle = Handle<Texture>;

class TextureManager : ResourceManagerBase<Texture> {
    using handle_type = Handle<Texture>; 
public:
    TextureManager(Device& device) : m_device(device) {}
    
    bool load(const std::string& name, 
        const std::string& filePath);

    virtual void release(const std::string& name) override;

private:
    void createTextureImage();

    Device& m_device;
};