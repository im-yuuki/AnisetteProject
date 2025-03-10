//
// Created by Yuuki on 09/03/2025.
//

#include "core.h"
#include <logging.h>

const auto logger = anisette::logging::get("video");
static std::shared_ptr<SDL_Window> window = nullptr;
constexpr uint32_t WINDOW_INIT_FLAGS = SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

namespace anisette::core::video {
    bool init_window() {
        SDL_Window *win = SDL_CreateWindow(
            "Anisette",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            1280, 720,
            WINDOW_INIT_FLAGS
            );
        if (win == nullptr) {
            logger->error("Initialize window failed: {}", SDL_GetError());
            return false;
        }
        window = std::shared_ptr<SDL_Window>(win, SDL_DestroyWindow);
        return true;
    }

    std::shared_ptr<SDL_Window> get_window() {
        if (window == nullptr) logger->error("Window is not initialized");
        return window;
    }
}