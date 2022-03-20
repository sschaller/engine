#pragma once

#include <memory>
#include <vector>

#include "Window.h"


class WindowManager final {
   public:
    WindowManager();

    ~WindowManager();

    Window *CreateWindow(std::string title, uint32_t width, uint32_t height);

    void PollEvents();

    bool ShouldQuit();

    std::vector<const char *> GetRequiredExtensions(Window *pWindow) const;

   private:
    bool quit_ = false;
    std::vector<std::unique_ptr<Window>> windows_;
};
