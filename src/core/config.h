//
// Created by Yuuki on 31/03/2025.
//
#pragma once
#include <cstdint>

namespace anisette::core::config {
    extern const uint32_t SDL_EVENT_SAVE_CONFIG_FAILURE;
    inline bool is_fallback = true;

    enum FPS_VALUE : int {
        // higher than 0 means custom FPS limit
        VSYNC = 0, // enable vsync and limit to display refresh rate
        UNLIMITED = -1, // no limit
        HALF_DISPLAY = -2, // limit to half of display refresh rate
        DISPLAY = -3, // limit to display refresh rate
        X2_DISPLAY = -4, // limit to double of display refresh rate
        X4_DISPLAY = -5, // limit to quadruple of display refresh rate
        X8_DISPLAY = -6, // limit to octuple of display refresh rate
    };

    enum DISPLAY_MODE : uint8_t {
        EXCLUSIVE = 0,
        WINDOWED = 1,
        BORDERLESS = 2,
    };

    // default values
    inline unsigned render_width = 1920;
    inline unsigned render_height = 1080;
    inline int fps = DISPLAY;
    inline DISPLAY_MODE display_mode = EXCLUSIVE;
    inline uint8_t sound_volume = 128;
    inline uint8_t music_volume = 128;
    inline bool enable_discord_rpc = true;

    extern bool load();
    extern bool save(bool quiet = false);
    extern void save_async();
}