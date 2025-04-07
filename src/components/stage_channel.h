//
// Created by Yuuki on 07/04/25.
//
#pragma once
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_render.h>
#include <queue>
#include "common.h"
#include "container.h"
#include "data.h"
#include "item.h"

#define NOTE_DISPLAY_RANGE 80

namespace anisette::components
{
    constexpr SDL_Color NOTE_COLOR      = {255, 255, 255, 180};
    constexpr SDL_Color NOTE_FAIL_COLOR = {128, 128, 128, 100};
    constexpr SDL_Color KEY_COLOR       = {160, 160, 160, 100};
    constexpr SDL_Color KEY_HOLD_COLOR  = {235, 0, 85, 255};
    constexpr SDL_Color KEY_TEXT_COLOR  = {255, 255, 255, 255};
    // constexpr SDL_Color KEY_HOLD_PERFECT_COLOR = {0, 168, 107, 255}; // green (#00A86B)
    // constexpr SDL_Color KEY_HOLD_GOOD_COLOR    = {0, 103, 165, 255}; // blue (#0067A5)
    // constexpr SDL_Color KEY_HOLD_OK_COLOR      = {255, 191, 0, 255}; // yellow (#FFBF00)
    // constexpr SDL_Color KEY_HOLD_FAIL_COLOR    = {240, 73, 35, 255}; // red (#F04923)

    class StageChannel final : public Container {
        struct NoteDisplayData {
            bool holded = false, failed = false;
            int start, end; // end = 0 means single tap note
            int start_display, end_display;

            NoteDisplayData(const int start, const int end, const int perfect_offset)
            : start(start), end(end), start_display(start - perfect_offset), end_display(end == 0 ? start + perfect_offset : end + perfect_offset) {}
        };

        utils::ScoreCalculator *score_calculator;
        int preview_size_ms;
        int current_music_pos_ms = -10000;
        int note_list_index = 0;
        unsigned key_press_count = 0;
        bool last_key_holding_state = false;
        std::vector<data::Note> *note_list;
        std::vector<NoteDisplayData> loaded_note;
        Text *key_text = nullptr;

        SDL_Rect get_note_draw_rect(const NoteDisplayData &note, const SDL_Rect note_display_rect) const {
            if (note.start_display < current_music_pos_ms + preview_size_ms) return {0, 0, 0, 0};
            if (note.end_display > current_music_pos_ms) return {0, 0, 0, 0};
            // calculate rect
            SDL_Rect ans = {note_display_rect.x, note_display_rect.y, note_display_rect.w, 0};
            int offset_y = (current_music_pos_ms + preview_size_ms - note.end_display) * note_display_rect.h / preview_size_ms;
            int size_y = (note.end_display - note.start_display) * note_display_rect.h / preview_size_ms;
            if (offset_y < 0) offset_y = 0;
            if (offset_y + size_y > note_display_rect.h) size_y = note_display_rect.h - offset_y;
            if (size_y < 0) return {0, 0, 0, 0};
            ans.y += offset_y;
            ans.h = size_y;
            return ans;
        }

    public:
        explicit StageChannel(utils::ScoreCalculator *score_calculator, std::vector<data::Note> *note_list)
            : score_calculator(score_calculator), note_list(note_list) {
            key_text = new Text("0", 24, KEY_TEXT_COLOR);
            preview_size_ms = score_calculator->base_offset_ms * 60;
        }

        void bind_value(const int current_music_pos_ms, const bool key_holding = false) {
            const bool ev_key_hold = key_holding && !last_key_holding_state;
            const bool ev_key_release = !key_holding && last_key_holding_state;

            // if start holding key
            if (ev_key_hold) key_press_count++;
            // handle key
            while (!loaded_note.empty()) {
                auto &note = loaded_note.front();
                const auto start_range = note.start_display - score_calculator->base_offset_ms * 5;
                const auto start_success = note.start_display - score_calculator->base_offset_ms * 2;
                const auto end_success = note.end_display + score_calculator->base_offset_ms * 2;
                const auto end_range = note.end_display + score_calculator->base_offset_ms * 5;
                if (current_music_pos_ms < start_range) continue;
                if (current_music_pos_ms < start_success) {
                    note.holded = true;
                    note.failed = true;
                    score_calculator->submit_note(current_music_pos_ms - note.start);
                } else if (current_music_pos_ms < end_success) {
                    if (!note.holded) {
                        note.holded = true;
                        score_calculator->submit_note(current_music_pos_ms - note.start);
                    } else if (note.end != 0 && !note.failed && ev_key_release) {
                        note.failed = true;
                        score_calculator->submit_note(note.end - current_music_pos_ms);
                    }
                } else if (!note.failed) {
                    note.failed = true;
                    if (note.end == 0 && !note.holded) score_calculator->submit_note(current_music_pos_ms - note.start);
                    else if (note.end != 0 && note.holded) score_calculator->submit_note(0);
                }
                if (current_music_pos_ms > end_range) loaded_note.erase(loaded_note.begin());
            }
            // load new notes
            while (note_list_index < note_list->size()) {
                auto &[start, end] = note_list->at(note_list_index);
                NoteDisplayData data = {start, end, score_calculator->base_offset_ms};
                if (data.start_display >= current_music_pos_ms + preview_size_ms) break;
                loaded_note.push_back(data);
            }
            this->current_music_pos_ms = current_music_pos_ms;
            last_key_holding_state = key_holding;

        }

        void draw(SDL_Renderer *renderer, SDL_Rect draw_rect, uint8_t alpha) override {
            if (hidden) return;
            SDL_Rect note_display_rect = {draw_rect.x, draw_rect.y, draw_rect.w, draw_rect.h * NOTE_DISPLAY_RANGE / 100};
            SDL_Rect key_display_rect = {draw_rect.x, draw_rect.y + note_display_rect.h, draw_rect.w, draw_rect.h * (100 - NOTE_DISPLAY_RANGE) / 100};
            SDL_SetRenderTarget(renderer, nullptr);
            // draw notes
            for (const auto &note : loaded_note) {
                const auto [x, y, w, h] = get_note_draw_rect(note, note_display_rect);
                if (w == 0 || h == 0) continue;
                int r = NOTE_COLOR.r, g = NOTE_COLOR.g, b = NOTE_COLOR.b, a = NOTE_COLOR.a;
                if (note.failed) {
                    r = NOTE_FAIL_COLOR.r;
                    g = NOTE_FAIL_COLOR.g;
                    b = NOTE_FAIL_COLOR.b;
                    a = NOTE_FAIL_COLOR.a;
                }
                roundedBoxRGBA(renderer, x, y, x + w, y + h, ROUNDED_RECTANGLE_RADIUS, r, g, b, a);
            }
            // draw key
            int r = KEY_COLOR.r, g = KEY_COLOR.g, b = KEY_COLOR.b, a = KEY_COLOR.a;
            if (last_key_holding_state) {
                r = KEY_HOLD_COLOR.r;
                g = KEY_HOLD_COLOR.g;
                b = KEY_HOLD_COLOR.b;
                a = KEY_HOLD_COLOR.a;
            }
            roundedBoxRGBA(renderer, key_display_rect.x, key_display_rect.y, key_display_rect.x + key_display_rect.w, key_display_rect.y + key_display_rect.h, ROUNDED_RECTANGLE_RADIUS, r, g, b, a);
        }

        void set_hidden(const bool state) override {
            hidden = state;
        }

        void hide_draw_rect() override;
        ~StageChannel() override;
    };
}