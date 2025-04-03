//
// Created by Yuuki on 31/03/2025.
//
#pragma once
#include "utils/common.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <atomic>
#include <cstdint>

#define FRT_OVERLAY_FONT_FILE "assets/fonts/Roboto-Bold.ttf"

namespace anisette::core
{
    extern uint64_t last_frame_time;
    extern std::atomic_bool stop_requested;
    extern uint64_t target_frame_time;

    extern bool init();
    extern void cleanup();
    extern void _main_loop();
} // namespace anisette::core

namespace anisette::core::video
{
    extern SDL_Window *window;
    extern SDL_Renderer *renderer;
    extern SDL_DisplayMode display_mode;
    /**
     * @brief Initialize the video module
     *
     * @return true if success, false otherwise
     */
    extern bool init();
    extern void cleanup();
} // namespace anisette::core::video

namespace anisette::core::audio
{
    extern bool init();
    extern void cleanup();
} // namespace anisette::core::audio
