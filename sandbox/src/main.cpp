#include <engine.h>

#include <iostream>

#include "render/passes/TrianglePass.h"
#include "render/RenderTarget.h"

class TriangleApp : public Application {
public:
    TriangleApp(int width, int height, std::string name) : Application(width, height, name) {}

protected:
    bool onCreate() override {
        LOG_TRACE("TriangleApp created!");
        
        return true;
    }

    bool onUpdate(double ts) override {
        // Update logic here
        return true;
    }

    bool onRender() override {
        // Render logic here
        
        return true;
    }

    bool onDestroy() override {
        std::cout << "TriangleApp destroyed!" << std::endl;
        return true;
    }
};

int main() {
    TriangleApp app(800, 600, "Hello Triangles");
    app.run();

    return 0;
}