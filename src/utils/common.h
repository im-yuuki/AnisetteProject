//
// Created by Yuuki on 01/04/2025.
//
#pragma once
#include <cstdint>
#include <climits>
#include <random>
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
        enum State : uint8_t {
            NONE = 0,
            PERFECT = 1,
            GOOD = 2,
            OK = 3,
            MISS = 4
        };
        explicit ScoreCalculator(const int base_offset_ms = 50, const int hp_drain = 0) : hp_drain(hp_drain), base_offset_ms(base_offset_ms) {}
        unsigned score = 0, combo = 0, note_count = 0;

        int hp = 200, hp_drain = 0;

        const int base_offset_ms = 50;
        int perfect_count = 0, // 30 point
            good_count = 0, // 10 point
            ok_count = 0, // 5 point
            miss_count = 0; // 0 point


        void submit_note(const int offset_ms) {
            note_count++;
            if (offset_ms > -base_offset_ms && offset_ms < base_offset_ms) {
                perfect_count++;
                score += 30 * ++combo;
                combine_state(PERFECT);
            }
            else if (offset_ms > -base_offset_ms * 2 && offset_ms < base_offset_ms * 2) {
                good_count++;
                score += 10 * ++combo;
                combine_state(GOOD);
            }
            else if (offset_ms > -base_offset_ms * 3 && offset_ms < base_offset_ms * 3) {
                ok_count++;
                score += 5 * combo;
                combine_state(OK);
            }
            else if (offset_ms > -base_offset_ms * 6 && offset_ms < base_offset_ms * 6) {
                miss_count++;
                combo = 0;
                hp -= hp_drain;
                if (hp < 0) hp = 0;
                combine_state(MISS);
            } else {
                // do not calculate
                note_count--;
            }
        }

        int get_accuracy_percentage() const {
            // div 100 to get integer part
            // mod 100 to get decimal part
            const int total = perfect_count + good_count + ok_count + miss_count;
            const int sum = perfect_count * 30 + good_count * 10 + ok_count * 5;
            if (total == 0) return 0;
            return sum * 10000 / total;
        }

        State get_last_state() {
            const auto ans = state;
            state = NONE;
            return ans;
        }
    private:
        void combine_state(const State new_state) {
            state = state > new_state ? state : new_state;
        }

        State state = NONE;
    };
} // namespace anisette::utils
