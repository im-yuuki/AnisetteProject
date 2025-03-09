//
// Created by Yuuki on 09/03/2025.
//

#include "core.h"

static std::shared_ptr<SDL_Window> window = nullptr;

namespace anisette::core::renderer {
    bool init_window() {
        SDL_Window *win = SDL_CreateWindow(
            "Anisette",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            1280, 720,
            SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
            );
        if (win == nullptr) return false;
        window = std::shared_ptr<SDL_Window>(win, SDL_DestroyWindow);
        return true;
    }

    std::shared_ptr<SDL_Window> get_window() {
        return window;
    }
}