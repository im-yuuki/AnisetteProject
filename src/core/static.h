//
// Created by Yuuki on 03/04/2025.
//
#pragma once
#include <string>
#include <cstdint>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

namespace anisette::core::abstract {
    class Screen {
    public:
        virtual ~Screen() = default;
        virtual void on_event(const uint64_t &now, const SDL_Event &event) = 0;
        virtual void on_focus(const uint64_t &now) = 0;
        virtual void update(const uint64_t &now) = 0;
    };
} // namespace anisette::core::abstract

namespace anisette::core
{
    extern uint64_t last_frame_time;
    inline uint64_t system_freq = SDL_GetPerformanceFrequency();
} // namespace anisette::core

namespace anisette::core::video
{
    extern SDL_Renderer *renderer;
    extern SDL_DisplayMode display_mode;
} // namespace anisette::core::video

namespace anisette::core::audio
{
    extern uint8_t music_volume;
    extern uint8_t sound_volume;
    extern std::string music_display_name;
    extern std::string music_path;
    extern int music_position_ms();
    extern int music_duration_ms;
    extern bool is_paused;
} // namespace anisette::core::audio