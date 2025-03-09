//
// Created by Yuuki on 19/02/2025.
//
#pragma once
#include <SDL2/SDL.h>
#include <memory>

namespace anisette::core {
    int start(int argc, char *argv[]);

    namespace renderer {
        bool init_window();
        std::shared_ptr<SDL_Window> get_window();
    }
}