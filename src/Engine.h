#pragma once

class DeviceContext;
class Window;

class Engine {
   public:
    Engine(DeviceContext &rContext, Window &rWindow);

    void Render();

   private:
    DeviceContext *pContext_;
    Window *pWindow_;
};
