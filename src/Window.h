#pragma once
#include <stdexcept>
#include <string>
#include <vector>

struct SDL_Window;

class Window {
   public:
    Window(SDL_Window *pWindow) : pWindow_(pWindow) {}
    SDL_Window *GetSDLWindow() { return pWindow_; }

   private:
    SDL_Window *pWindow_;
};