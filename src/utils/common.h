//
// Created by Yuuki on 01/04/2025.
//
#pragma once
#include <cstdint>
#include <climits>
#include <random>

namespace anisette::utils
{
    inline std::mt19937_64 generator(std::random_device{}());

    inline uint8_t calc_fade_alpha(const bool reverse, const uint64_t start_point, const uint64_t now,
                               const uint64_t duration) {
        if (now < start_point)
            return reverse ? 255 : 0;
        if (now > start_point + duration)
            return reverse ? 0 : 255;
        const auto alpha = static_cast<int>((now - start_point) * 255 / duration);
        return reverse ? 255 - alpha : alpha;
    }

    inline int randint(const int min = INT_MIN, const int max = INT_MAX) {
        std::uniform_int_distribution distribution(min, max);
        return distribution(generator);
    }
} // namespace anisette::utils
