//
// Created by Yuuki on 01/04/2025.
//
#pragma once
#include <cstdint>
#include <climits>
#include <random>
#include <string>
#include <SDL2/SDL_rect.h>

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

    inline bool check_point_in_rect(const int x, const int y, const SDL_Rect &rect) {
        return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
    }

    class ScoreCalculator {
    public:
        explicit ScoreCalculator(const int base_offset_ms = 50, const int hp_drain = 0) : hp_drain(hp_drain), base_offset_ms(base_offset_ms) {}
        unsigned score = 0, combo = 0, note_count = 0, success = 0;

        int hp = 100;
        int hp_drain = 0;
        const int base_offset_ms;

        void submit_success() {
            note_count++;
            success++;
            score += 10 * ++combo;
        }

        void submit_fail() {
            note_count++;
            combo = 0;
            hp -= hp_drain;
            if (hp < 0) hp = 0;
        }

        std::string get_score_string() const {
            // fixed to 8-digit string
            std::string score_str = std::to_string(score);
            while (score_str.length() < 8) score_str = "0" + score_str;
            return score_str;
        }

        std::string get_combo_string() const {
            if (combo == 0) return "0x";
            return std::to_string(combo) + "x";
        }

        std::string get_accuracy_percentage_string() const {
            if (note_count == 0) return "100.0%";
            const auto accuracy = success * 10000 / note_count;
            return std::to_string(accuracy / 100) + "." + std::to_string(accuracy % 100) + "%";
        }

    };
} // namespace anisette::utils
