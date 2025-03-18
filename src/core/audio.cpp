//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include <logging.h>
#include <SDL3_mixer/SDL_mixer.h>

const auto logger = anisette::logging::get("audio");
constexpr SDL_AudioSpec desired {MIX_DEFAULT_FORMAT, 2, 44100 };

namespace anisette::core::audio
{
    bool init() {
        logger->info("SDL_mixer version: {}.{}.{}", SDL_MIXER_MAJOR_VERSION, SDL_MIXER_MINOR_VERSION, SDL_MIXER_MICRO_VERSION);
        if (!Mix_OpenAudio(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired)) {
            logger->error("Failed to initialize audio mixer!");
            return false;
        }
        return true;
    }

    void cleanup() {
        Mix_Quit();
    }
}