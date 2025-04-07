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
    uint32_t MUSIC_FINISHED_EVENT_ID = SDL_RegisterEvents(1);

    static Mix_Music *current_music = nullptr;

    // metadata for current music
    std::string music_display_name;
    std::string music_path;
    int music_duration_ms = 0;

    // frequently used sound
    Mix_Chunk *click_sound = nullptr;
    Mix_Chunk *hit_sound = nullptr;

    void play_click_sound() {
        // allocate channel 1 to click sound
        play_sound(click_sound, 1);
    }

    void play_hit_sound() {
        // allocate channel 2 to hit sound
        play_sound(hit_sound, 2);
    }

    uint8_t music_volume() {
        return Mix_VolumeMusic(-1);
    };

    uint8_t sound_volume() {
        return Mix_Volume(-1, -1);
    }

    bool is_paused() {
        return Mix_PausedMusic();
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

    bool init() {
        logger->debug("Opening audio device");
        if (Mix_OpenAudioDevice(44100, MIX_DEFAULT_FORMAT, 2, 1024, nullptr, 0)) {
            logger->error("Failed to open audio device");
            return false;
        }
        click_sound = Mix_LoadWAV("assets/sound/click.wav");
        hit_sound = Mix_LoadWAV("assets/sound/hitsound.wav");

        set_music_volume(config::music_volume);
        set_sound_volume(config::sound_volume);

        Mix_HookMusicFinished([]() {
            logger->debug("Music finished");
            SDL_PushEvent(new SDL_Event {.type = MUSIC_FINISHED_EVENT_ID});
        });

        return true;
    }

    void cleanup() {
        // save state
        config::music_volume = music_volume();
        config::sound_volume = sound_volume();

        Mix_FreeChunk(click_sound);
        Mix_FreeChunk(hit_sound);
        Mix_FreeMusic(current_music);
        Mix_CloseAudio();
    }

    void set_music_volume(uint8_t volume) {
        if (volume > MIX_MAX_VOLUME) volume = MIX_MAX_VOLUME;
        // logger->debug("Setting music volume to {}", volume);
        Mix_VolumeMusic(volume);
    }

    void set_sound_volume(uint8_t volume) {
        if (volume > MIX_MAX_VOLUME) volume = MIX_MAX_VOLUME;
        // logger->debug("Setting sound volume to {}", volume);
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
            Mix_FreeMusic(current_music);
            return false;
        }
        // fetch music metadata
        if (display_name.empty()) {
            music_display_name = Mix_GetMusicTitle(current_music);
        } else {
            music_display_name = display_name;
        }
        music_path = path;
        music_duration_ms = Mix_MusicDuration(current_music) * 1000;
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
        Mix_FreeMusic(current_music);
        current_music = nullptr;
        music_display_name = "";
        music_path = "";
    }

    void seek_music(int position_ms) {
        if (current_music == nullptr) return;
        if (position_ms < 0 || position_ms > music_duration_ms) return;
        logger->debug("Seeking music: {} to {}ms", music_display_name, position_ms);
        Mix_SetMusicPosition(static_cast<double>(position_ms) / 1000);
        if (is_paused()) resume_music();
    }
}