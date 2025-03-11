//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include <logging.h>

const auto logger = anisette::logging::get("loader");
constexpr uint32_t INIT_SUBSYSTEMS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS;

namespace anisette::core {
    int run(int argc, char *argv[]) {
        logger->info("Starting core...");
        if (SDL_InitSubSystem(INIT_SUBSYSTEMS) != 0) {
            logger->error("Initialize SDL failed: {}", SDL_GetError());
            return 1;
        }
        if (!video::init_window()) return 1;
        if (!audio::init()) return 1;

        // SDL_Delay(30000);
        logger->info("Start cleanup task");
        SDL_DestroyWindow(video::get_window().get());
        SDL_QuitSubSystem(INIT_SUBSYSTEMS);
        SDL_Quit();
        return 0;
    }
}