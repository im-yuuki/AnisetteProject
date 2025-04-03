//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include "internal.h"
#include "config.h"
#include "utils/logging.h"
#include <SDL2/SDL_mixer.h>

const auto logger = anisette::logging::get("audio");

namespace anisette::core::audio
{
    static Mix_Music *_current_music = nullptr;
    static std::string name;

    bool init() {
        logger->debug("Opening audio device");
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

    Mix_Chunk* load_sound(const std::string &path) {
        Mix_Chunk *sound = Mix_LoadWAV(path.c_str());
        if (sound == nullptr) {
            logger->error("Failed to load sound file: {}", path);
            return nullptr;
        }
        return sound;
    }

    bool play_sound(Mix_Chunk *sound, const int channel) {
        if (sound == nullptr) return false;
        if (Mix_PlayChannel(channel, sound, 0) == -1) {
            logger->error("Failed to play sound: {}", SDL_GetError());
            return false;
        }
        return true;
    }

    CurrentMusicInfo get_current_music_info() {
        CurrentMusicInfo info;
        if (_current_music == nullptr) return info;
        info.display_name = name;
        info.path = Mix_GetMusicTitle(_current_music);
        info.current_position_ms = 1000 * Mix_GetMusicPosition(_current_music);
        info.total_duration_ms = 1000 * Mix_MusicDuration(_current_music);
        info.paused = Mix_PausedMusic();
        return info;
    }

    bool play_music(const std::string &path, const std::string &display_name) {
        if (path.empty()) return false;
        stop_music();
        _current_music = Mix_LoadMUS(path.c_str());
        if (_current_music == nullptr) {
            logger->error("Failed to load music file: {}", path);
            stop_music();
            return false;
        }
        if (Mix_PlayMusic(_current_music, 0)) {
            logger->error("Failed to play music: {}", SDL_GetError());
            stop_music();
            return false;
        }
        if (display_name.empty()) {
            name = Mix_GetMusicTitle(_current_music);
        } else {
            name = display_name;
        }
        logger->debug("Playing music: {}", name);
        return true;
    }

    void pause_music() {
        if (_current_music == nullptr) return;
        logger->debug("Pausing music: {}", name);
        Mix_PauseMusic();
    }

    void resume_music() {
        if (_current_music == nullptr) return;
        logger->debug("Resuming music: {}", name);
        Mix_ResumeMusic();
    }

    void stop_music() {
        if (_current_music == nullptr) return;
        logger->debug("Stopping music: {}", name);
        Mix_HaltMusic();
        Mix_FreeMusic(_current_music);
        name = "";
    }
}