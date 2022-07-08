#include "WindowManager.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <stdexcept>

WindowManager::WindowManager() { SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS); }

WindowManager::~WindowManager() {
    for (auto &rWindow : windows_) {
        SDL_DestroyWindow(rWindow->GetSDLWindow());
    }
    SDL_Quit();
}

Window *WindowManager::CreateWindow(std::string title, uint32_t width, uint32_t height) {
    SDL_Window *pWindow = SDL_CreateWindow(title.c_str(),                              // Window title
                                           SDL_WINDOWPOS_CENTERED,                     // Initial x position
                                           SDL_WINDOWPOS_CENTERED,                     // Initial y position
                                           width,                                      // Initial width (px)
                                           height,                                     // Initial height (px)
                                           SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);  // Flags

    if (pWindow == nullptr) {
        throw std::runtime_error(std::string("Failed to create window: ") + SDL_GetError());
    }

    std::unique_ptr<Window> &rWindow = windows_.emplace_back(new Window(pWindow));
    return rWindow.get();
}

void WindowManager::PollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quit_ = true;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    for (std::unique_ptr<Window> &rspWindow : windows_) {
                        if (event.window.windowID == SDL_GetWindowID(rspWindow->GetSDLWindow())) {
                            rspWindow->OnResize();
                            break;
                        }
                    }
                }
                break;
        }
    }

    SDL_Delay(10);
}

bool WindowManager::ShouldQuit() { return quit_; }

/**
 * As of vulkan 2.0.20, window parameter is still needed.
 *
 * @param pWindow                       Pointer to window
 * @return std::vector<const char *>    Vector of extensions
 */
std::vector<const char *> WindowManager::GetRequiredExtensions(Window *pWindow) const {
    uint32_t count;
    if (!SDL_Vulkan_GetInstanceExtensions(pWindow->GetSDLWindow(), &count, nullptr)) {
        throw std::runtime_error("Failed to retrieve SDL Vulkan Extensions");
    }
    std::vector<const char *> extensions;
    extensions.resize(count);
    if (!SDL_Vulkan_GetInstanceExtensions(pWindow->GetSDLWindow(), &count, extensions.data())) {
        throw std::runtime_error("Failed to retrieve SDL Vulkan Extensions");
    }
    return extensions;
}