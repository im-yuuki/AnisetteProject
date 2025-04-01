//
// Created by Yuuki on 31/03/2025.
//
#pragma once
#include "components/frametime_overlay.h"
#include <SDL2/SDL_video.h>
#include <atomic>
#include <cstdint>

namespace anisette::core
{
    /**
     * @brief The frequency of the system's performance counter.
     *
     * This is simply value of `SDL_GetPerformanceFrequency()`.
     * I keep it here for easy access and avoid calling the function multiple times.
     */
    inline std::atomic_bool _stop_requested = false;
    inline uint64_t _target_frame_time = 0;
    inline components::FrameTimeOverlay *frame_time_overlay = nullptr;

    // const uint64_t system_freq = SDL_GetPerformanceFrequency();

    extern bool init();
    extern void cleanup();

    extern void _main_loop();

} // namespace anisette::core

namespace anisette::core::video
{
    inline SDL_Window *window = nullptr;
    inline SDL_Renderer *renderer = nullptr;
    inline SDL_DisplayMode display_mode {};
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
