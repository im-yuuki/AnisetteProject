//
// Created by Yuuki on 31/03/2025.
//
#pragma once
#include <cstdint>

namespace anisette::core::config {
    extern const uint32_t SDL_EVENT_SAVE_CONFIG_FAILURE;

    typedef int FPS_VALUE;
    constexpr FPS_VALUE VSYNC = 0;
    constexpr FPS_VALUE UNLIMITED = -1;
    constexpr FPS_VALUE HALF_DISPLAY = -2;
    constexpr FPS_VALUE DISPLAY = -3;
    constexpr FPS_VALUE X2_DISPLAY = -4;
    constexpr FPS_VALUE X4_DISPLAY = -5;
    constexpr FPS_VALUE X8_DISPLAY = -6;

    enum DISPLAY_MODE : uint8_t {
        EXCLUSIVE = 0,
        WINDOWED = 1,
        BORDERLESS = 2,
    };

    extern bool is_fallback;
    // default values
    extern int render_width;
    extern int render_height;
    extern int fps;
    extern DISPLAY_MODE display_mode;
    extern uint8_t sound_volume;
    extern uint8_t music_volume;
    extern bool enable_discord_rpc;
    extern bool show_frametime_overlay;

    extern bool load();
    extern bool save(bool quiet = false);
}