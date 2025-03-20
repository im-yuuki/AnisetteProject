//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include <logging.h>
#include <SDL2/SDL_mixer.h>

const auto logger = anisette::logging::get("audio");

namespace anisette::core::audio
{
    bool init() {
        logger->info("SDL_mixer version: {}.{}.{}", SDL_MIXER_MAJOR_VERSION, SDL_MIXER_MINOR_VERSION, SDL_MIXER_PATCHLEVEL);
        if (Mix_OpenAudioDevice(44100, MIX_DEFAULT_FORMAT, 2, 1024, nullptr, 0)) {
            logger->error("Failed to initialize audio mixer!");
            return false;
        }
        return true;
    }

    void cleanup() {
        Mix_Quit();
    }
}