#pragma once

#include "Swapchain.h"

class DeviceContext;
class Window;

class Engine {
public:
    Engine(DeviceContext &rContext, Window &rWindow);
    ~Engine();

    void Render();

private:
    DeviceContext &rContext_;
    Window &rWindow_;
    Swapchain swapchain_;
};
