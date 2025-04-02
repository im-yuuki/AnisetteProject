//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include "_internal.h"
#include "config.h"
#include "utils/logging.h"
#include <SDL2/SDL_mixer.h>

const auto logger = anisette::logging::get("audio");

namespace anisette::core::audio
{
    static Mix_Music *_current_music = nullptr;

    bool init() {
        if (Mix_OpenAudioDevice(44100, MIX_DEFAULT_FORMAT, 2, 1024, nullptr, 0)) {
            logger->error("Failed to open audio device");
            return false;
        }
        set_music_volume(config::music_volume);
        set_sound_volume(config::sound_volume);
        return true;
    }

    void cleanup() {
        Mix_CloseAudio();
    }

    void set_music_volume(const uint8_t volume) {
        Mix_VolumeMusic(volume);
    }

    void set_sound_volume(const uint8_t volume) {
        Mix_Volume(-1, volume);
    }

    Mix_Chunk* load_sound(const char* path) {
        Mix_Chunk *sound = Mix_LoadWAV(path);
        if (sound == nullptr) {
            logger->error("Failed to load sound file: {}", path);
            return nullptr;
        }
        return sound;
    }

    bool play_sound(Mix_Chunk *sound) {
        if (Mix_PlayChannel(-1, sound, 0) == -1) {
            logger->error("Failed to play sound: {}", SDL_GetError());
            return false;
        }
        return true;
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