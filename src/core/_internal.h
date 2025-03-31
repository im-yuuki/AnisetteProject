//
// Created by Yuuki on 31/03/2025.
//
#pragma once
#include <SDL2/SDL_video.h>
#include <cstdint>

namespace anisette::core
{
    /**
 * @brief The frequency of the system's performance counter.
 *
 * This is simply value of `SDL_GetPerformanceFrequency()`.
 * I keep it here for easy access and avoid calling the function multiple times.
 */
    extern const uint64_t system_freq;
    extern void _handle_event(const uint64_t &start_frame);
    extern void _handle_frame(const uint64_t &start_frame);

}

namespace anisette::core::video
{
    /**
     * @brief Global OpenGL context
     */
    inline SDL_GLContext gl_context = nullptr;
    inline SDL_Window *window = nullptr;
    inline SDL_DisplayMode display_mode;

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
}