//
// Created by Yuuki on 03/04/2025.
//
#include "data.h"

namespace anisette::screens
{
    class ScoreCalculator {
    public:
        explicit ScoreCalculator(const int base_offset_ms = 50, const int hp_drain = 0) : hp_drain(hp_drain), base_offset_ms(base_offset_ms) {}
        unsigned score = 0, combo = 0, note_count = 0;

        int hp = 100, hp_drain = 0;

        int base_offset_ms = 50;
        int perfect_count = 0, // 30 point
            good_count = 0, // 10 point
            ok_count = 0, // 5 point
            miss_count = 0; // 0 point


        void submit_note(const int offset_ms) {
            note_count++;
            if (offset_ms > -base_offset_ms && offset_ms < base_offset_ms) {
                perfect_count++;
                score += 30 * ++combo;
            }
            else if (offset_ms > -base_offset_ms * 2 && offset_ms < base_offset_ms * 2) {
                good_count++;
                score += 10 * ++combo;
            }
            else if (offset_ms > -base_offset_ms * 3 && offset_ms < base_offset_ms * 3) {
                ok_count++;
                score += 5 * combo;
            }
            else {
                miss_count++;
                combo = 0;
                hp -= hp_drain;
                if (hp < 0) hp = 0;
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
    };
    class StageChannel {

    };
}