#pragma once
#include <stdexcept>
#include <string>
#include <vector>
#include <SDL2/SDL_vulkan.h>

class DeviceContext;
struct SDL_Window;

class Window {
public:
    struct Size {
        uint32_t width;
        uint32_t height;
    };

public:
    Window(SDL_Window *pWindow);
    SDL_Window *GetSDLWindow() { return pWindow_; }
    Size GetSize() const;
    VkSurfaceKHR CreateSurface(DeviceContext &rDeviceContext);

private:
    SDL_Window *pWindow_;
};