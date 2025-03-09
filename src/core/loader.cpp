//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include <logging.h>
#include <SDL2/SDL.h>
using namespace anisette::core;

const auto logger = anisette::logging::get("core");

bool init() {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        logger->error("Initialize SDL failed: {}", SDL_GetError());
        return false;
    }
}