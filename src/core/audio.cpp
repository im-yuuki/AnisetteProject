//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include "internal.h"
#include "config.h"
#include "logging.h"
#include <SDL2/SDL_mixer.h>
#include <atomic>

const auto logger = anisette::logging::get("audio");

namespace anisette::core::audio
{
    static Mix_Music *current_music = nullptr;
    static std::atomic_bool music_halted_flag = false;
    std::string music_display_name;
    uint8_t music_volume = 0;
    uint8_t sound_volume = 0;

    bool init() {
        logger->debug("Opening audio device");
        if (Mix_OpenAudioDevice(44100, MIX_DEFAULT_FORMAT, 2, 1024, nullptr, 0)) {
            logger->error("Failed to open audio device");
            return false;
        }
        set_music_volume(config::music_volume);
        set_sound_volume(config::sound_volume);
        Mix_HookMusicFinished([]() {
            logger->debug("Music finished");
            if (current_music != nullptr) {
                Mix_FreeMusic(current_music);
                current_music = nullptr;
                music_display_name = "";
            }
        });
        return true;
    }

    void cleanup() {
        Mix_CloseAudio();
    }

    void set_music_volume(uint8_t volume) {
        if (volume > MIX_MAX_VOLUME) volume = MIX_MAX_VOLUME;
        music_volume = volume;
        Mix_VolumeMusic(volume);
    }

    void set_sound_volume(uint8_t volume) {
        if (volume > MIX_MAX_VOLUME) volume = MIX_MAX_VOLUME;
        sound_volume = volume;
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

    int music_position_ms() {
        if (current_music == nullptr)
            return 0;
        const int position = Mix_GetMusicPosition(current_music);
        if (position == -1) {
            logger->error("Failed to get music position: {}", SDL_GetError());
            return 0;
        }
        return position * 1000;
    }

    bool play_music(const std::string &path, const std::string &display_name) {
        if (path.empty()) return false;
        stop_music();
        current_music = Mix_LoadMUS(path.c_str());
        if (current_music == nullptr) {
            logger->error("Failed to load music file: {}", path);
            return false;
        }
        if (Mix_PlayMusic(current_music, 0)) {
            logger->error("Failed to play music: {}", SDL_GetError());
            stop_music();
            return false;
        }
        if (display_name.empty()) {
            music_display_name = Mix_GetMusicTitle(current_music);
        } else {
            music_display_name = display_name;
        }
        logger->debug("Playing music: {}", music_display_name);
        return true;
    }

    void pause_music() {
        if (current_music == nullptr) return;
        logger->debug("Pausing music: {}", music_display_name);
        Mix_PauseMusic();
    }

    void resume_music() {
        if (current_music == nullptr) return;
        logger->debug("Resuming music: {}", music_display_name);
        Mix_ResumeMusic();
    }

    void stop_music() {
        if (current_music == nullptr) return;
        logger->debug("Stopping music: {}", music_display_name);
        if (music_position_ms() > 0 && ) {
            music_halted_flag = true;
        }
        Mix_HaltMusic();
        Mix_FreeMusic(current_music);
        current_music = nullptr;
        music_display_name = "";
    }
}