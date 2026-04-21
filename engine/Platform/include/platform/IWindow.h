#pragma once

#include <memory>

#include "ISurface.h"

namespace platform {

class IWindow {
public:
    virtual ~IWindow() = default;

    virtual bool shouldClose() const = 0;
    virtual void pollEvents() const = 0;
    virtual void getFramebufferSize(int& w, int& h) const = 0;

    virtual std::unique_ptr<ISurface> createSurface() = 0;
};

}; // namespace platform