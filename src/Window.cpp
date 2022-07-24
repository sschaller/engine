#include "Window.h"

#include "DeviceContext.h"

Window::Window(SDL_Window *pWindow) : pWindow_(pWindow) {}

Window::Size Window::GetSize() const {
    int width, height;
    SDL_Vulkan_GetDrawableSize(pWindow_, &width, &height);

    return Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

VkSurfaceKHR Window::CreateSurface(DeviceContext &rDeviceContext) {
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(pWindow_, rDeviceContext.GetInstance(), &surface)) {
        throw std::runtime_error(std::string("Failed to create surface for window: ") + SDL_GetError());
    }
    return surface;
}

void Window::OnResize() {
    if (resizeCallback_ != nullptr) {
        resizeCallback_();
    }
}

void Window::RegisterResizeCallback(Window::ResizeCallback &&rrFunc) { resizeCallback_ = std::move(rrFunc); }
