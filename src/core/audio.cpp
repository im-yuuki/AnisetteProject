//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include <logging.h>
#include <SDL2/SDL_mixer.h>

const auto logger = anisette::logging::get("audio");

namespace anisette::core::audio
{
    static Mix_Music *_current_music = nullptr;

    bool init() {
        set_music_volume(config::music_volume);
        set_sound_volume(config::sound_volume);
        return true;
    }

    void cleanup() {
    }

    void set_music_volume(const uint8_t volume) {
        Mix_VolumeMusic(volume);
    }

    void set_sound_volume(const uint8_t volume) {
        Mix_Volume(-1, volume);
    }

    bool play_music(const char* path) {
        stop_music();
        _current_music = Mix_LoadMUS(path);
        if (_current_music == nullptr) {
            logger->error("Failed to load music file: {}", path);
            return false;
        }
        if (!Mix_PlayMusic(_current_music, 0)) {
            logger->error("Failed to play music: {}", SDL_GetError());
        }
        logger->debug("Playing music: {}", Mix_GetMusicTitle(_current_music));
        return true;
    }

    void pause_music() {
        if (_current_music == nullptr) return;
        logger->debug("Pausing music: {}", Mix_GetMusicTitle(_current_music));
        Mix_PauseMusic();
    }

    void resume_music() {
        if (_current_music == nullptr) return;
        logger->debug("Resuming music: {}", Mix_GetMusicTitle(_current_music));
        Mix_ResumeMusic();
    }

    void stop_music() {
        if (_current_music == nullptr) return;
        logger->debug("Stopping music: {}", Mix_GetMusicTitle(_current_music));
        Mix_HaltMusic();
        Mix_FreeMusic(_current_music);
    }
}