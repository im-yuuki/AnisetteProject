//
// Created by Yuuki on 31/03/2025.
//
#pragma once
#include "frametime_overlay.h"
#include "background.h"
#include "common.h"
#include <SDL2/SDL_video.h>
#include <atomic>
#include <cstdint>

namespace anisette::core
{
    extern std::atomic_bool stop_requested;
    extern uint64_t target_frame_time;
    extern components::FrameTimeOverlay *frame_time_overlay;
    extern components::Background *background_instance;

    extern bool init();
    extern void cleanup();
    extern void main_loop();
} // namespace anisette::core

namespace anisette::core::video
{
    extern SDL_Window *window;

    extern bool init();
    extern void cleanup();
} // namespace anisette::core::video

namespace anisette::core::audio
{
    extern bool init();
    extern void cleanup();
} // namespace anisette::core::audio
