//
// Created by Yuuki on 01/04/2025.
//
#pragma once
#include <SDL2/SDL_timer.h>
#include <cstdint>

namespace anisette::utils
{
    inline uint64_t system_freq = SDL_GetPerformanceFrequency();

    inline uint8_t calc_fade_alpha(const bool reverse, const uint64_t start_point, const uint64_t now,
                               const uint64_t duration) {
        if (now < start_point)
            return reverse ? 255 : 0;
        if (now > start_point + duration)
            return reverse ? 0 : 255;
        const auto alpha = static_cast<int>((now - start_point) * 255 / duration);
        return reverse ? 255 - alpha : alpha;
    }
} // namespace anisette::utils
