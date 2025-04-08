//
// Created by Yuuki on 07/04/25.
//
#pragma once
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_render.h>
#include <queue>
#include "core.h"
#include "common.h"
#include "container.h"
#include "data.h"
#include "item.h"

#define NOTE_DISPLAY_RANGE 85
#define NOTE_DISPLAY_FONT_SIZE 32
#define NOTE_DISPLAY_SIZE 10 // screen = n * note size

namespace anisette::components
{
    constexpr SDL_Color NOTE_COLOR      = {255, 255, 255, 180};
    constexpr SDL_Color NOTE_FAIL_COLOR = {128, 128, 128, 100};
    constexpr SDL_Color KEY_COLOR       = {160, 160, 160, 100};
    constexpr SDL_Color KEY_HOLD_COLOR  = {235, 0, 85, 255};
    constexpr SDL_Color KEY_TEXT_COLOR  = {255, 255, 255, 255};

    class StageChannel final : public Container {
        struct NoteDisplayData {
            bool processed = false, failed = false;
            int start, end;
        };

        utils::ScoreCalculator *score_calculator;
        int preview_size_ms;
        int note_list_index = 0;
        unsigned key_press_count = 0;
        bool last_key_holding_state = false;
        std::vector<data::Note> *note_list;
        std::deque<NoteDisplayData> loaded_note;
        Text *key_text = nullptr;

        int current_music_pos_ms = -10000;
        bool ev_key_down = false;

        [[nodiscard]]
        SDL_Rect get_note_draw_rect(const NoteDisplayData &note, const SDL_Rect note_display_rect) const {
            if (note.start > current_music_pos_ms + preview_size_ms) return {0, 0, 0, 0};
            if (note.end < current_music_pos_ms) return {0, 0, 0, 0};
            // calculate rect
            SDL_Rect ans = {note_display_rect.x, note_display_rect.y, note_display_rect.w, 0};
            const int offset_y = (current_music_pos_ms + preview_size_ms - note.end) * note_display_rect.h / preview_size_ms;
            int size_y = (note.end - note.start) * note_display_rect.h / preview_size_ms;
            if (offset_y + size_y > note_display_rect.h) size_y = note_display_rect.h - offset_y;
            ans.y += offset_y;
            ans.h = size_y;
            return ans;
        }

    public:
        bool finished = false;

        explicit StageChannel(utils::ScoreCalculator *score_calculator, std::vector<data::Note> *note_list, const std::string &init_text)
            : score_calculator(score_calculator), note_list(note_list) {
            key_text = new Text(init_text, NOTE_DISPLAY_FONT_SIZE, KEY_TEXT_COLOR);
            preview_size_ms = score_calculator->base_offset_ms * NOTE_DISPLAY_SIZE;
        }

        void bind_value(const int current_music_pos_ms, const bool key_holding = false) {
            this->current_music_pos_ms = current_music_pos_ms;
            ev_key_down = key_holding && !last_key_holding_state;
            last_key_holding_state = key_holding;
        }

        void draw(SDL_Renderer *renderer, const SDL_Rect draw_rect, const uint8_t alpha) override {
            if (hidden) return;
            if (loaded_note.empty() && note_list_index >= note_list->size()) finished = true;
            // draw border lines
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_RenderDrawLine(renderer, draw_rect.x, draw_rect.y, draw_rect.x, draw_rect.y + draw_rect.h);
            SDL_RenderDrawLine(renderer, draw_rect.x + draw_rect.w, draw_rect.y, draw_rect.x + draw_rect.w, draw_rect.y + draw_rect.h);
            // split rect
            const SDL_Rect note_display_rect = {draw_rect.x, draw_rect.y, draw_rect.w, draw_rect.h * NOTE_DISPLAY_RANGE / 100};
            const SDL_Rect key_display_rect = {draw_rect.x, draw_rect.y + note_display_rect.h, draw_rect.w, draw_rect.h * (100 - NOTE_DISPLAY_RANGE) / 100};
            // if key down
            if (ev_key_down) {
                key_press_count++;
                for (auto &[processed, failed, start, end] : loaded_note) {
                    if (start > current_music_pos_ms + score_calculator->base_offset_ms) break;
                    if (processed) continue;
                    processed = true;
                    if (start <= current_music_pos_ms && current_music_pos_ms <= end) {
                        core::audio::play_hit_sound();
                        score_calculator->submit_success();
                    } else {
                        failed = true;
                        score_calculator->submit_fail();
                    }
                    break;
                }
            }
            // draw notes
            for (const auto &note : loaded_note) {
                const auto rect = get_note_draw_rect(note, note_display_rect);
                if (rect.w == 0 || rect.h == 0) continue;
                int r = NOTE_COLOR.r, g = NOTE_COLOR.g, b = NOTE_COLOR.b, a = NOTE_COLOR.a;
                if (note.failed) {
                    r = NOTE_FAIL_COLOR.r;
                    g = NOTE_FAIL_COLOR.g;
                    b = NOTE_FAIL_COLOR.b;
                    a = NOTE_FAIL_COLOR.a;
                }
                SDL_SetRenderDrawColor(renderer, r, g, b, a);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(renderer, &rect);
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
            if (key_press_count > 0) key_text->change_text(std::to_string(key_press_count));
            key_text->draw(renderer, key_display_rect, false);
            // delete old notes
            while (!loaded_note.empty()) {
                auto front = loaded_note.front();
                if (front.end > current_music_pos_ms) break;
                if (!front.processed) score_calculator->submit_fail();
                loaded_note.pop_front();
            }
            // load new notes
            while (note_list_index < note_list->size()) {
                auto &note = note_list->at(note_list_index);
                if (note.start > current_music_pos_ms + preview_size_ms) break;
                loaded_note.push_back({
                    false, false,
                    note.start - score_calculator->base_offset_ms,
                    note.start + score_calculator->base_offset_ms
                });
                note_list_index++;
            }
        }

        void set_hidden(const bool state) override {
            hidden = state;
        }

        void hide_draw_rect() override {};
        ~StageChannel() override {};
    };
}