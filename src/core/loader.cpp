//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include <logging.h>

const auto logger = anisette::logging::get("core");

bool init;

namespace anisette::core {
    int start(int argc, char *argv[]) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
            logger->error("Initialize SDL failed: {}", SDL_GetError());
            return 1;
        }
        if (!renderer::init_window()) {
            logger->error("Initialize renderer failed: {}", SDL_GetError());
            return 1;
        }
        SDL_Delay(30000);
        SDL_DestroyWindow(renderer::get_window().get());
        SDL_Quit();
        return 0;
    }
}