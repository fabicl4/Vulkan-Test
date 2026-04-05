#include <iostream>

#include <engine.h>

class TestApp : public Application {
public:
    TestApp(int width, int height, std::string name) : Application(width, height, name) {}

protected:
    bool onCreate() override {
        std::cout << "TestApp created!" << std::endl;
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
};

int main() {
    TestApp app(800, 600, "Vulkan Test");
    app.run();

    return 0;
}