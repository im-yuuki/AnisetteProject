//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include <logging.h>
#include <SDL2/SDL_mixer.h>

const auto logger = anisette::logging::get("audio");
static Mix_Music *current_music = nullptr;

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

    void set_music_volume(const int volume) {
        Mix_VolumeMusic(volume);
    }

    void set_sound_volume(const int volume) {
        Mix_Volume(-1, volume);
    }

    bool play_music(const char* path) {
        stop_music();
        current_music = Mix_LoadMUS(path);
        if (current_music == nullptr) {
            logger->error("Failed to load music file: {}", path);
            return false;
        }
        if (!Mix_PlayMusic(current_music, 0)) {
            logger->error("Failed to play music: {}", SDL_GetError());
        }
        logger->debug("Playing music: {}", Mix_GetMusicTitle(current_music));
        return true;
    }

    void pause_music() {
        if (current_music == nullptr) return;
        logger->debug("Pausing music: {}", Mix_GetMusicTitle(current_music));
        Mix_PauseMusic();
    }

    void resume_music() {
        if (current_music == nullptr) return;
        logger->debug("Resuming music: {}", Mix_GetMusicTitle(current_music));
        Mix_ResumeMusic();
    }

    void stop_music() {
        if (current_music == nullptr) return;
        logger->debug("Stopping music: {}", Mix_GetMusicTitle(current_music));
        Mix_HaltMusic();
        Mix_FreeMusic(current_music);
    }
}