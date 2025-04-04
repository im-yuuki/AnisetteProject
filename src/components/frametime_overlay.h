//
// Created by Yuuki on 03/04/2025.
//
#pragma once
#include "core.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>

#define FRT_OVERLAY_FONT_SIZE_1 16
#define FRT_OVERLAY_FONT_SIZE_2 12
#define FRT_OVERLAY_UPDATE_INTERVAL 50 // ms

namespace anisette::components
{
    class FrameTimeOverlay {
    public:
        explicit FrameTimeOverlay(SDL_Renderer *renderer) : renderer(renderer) {
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, src_rect.w, src_rect.h);
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        };

        void render_text(const std::string &text, const int font_size, const SDL_Color &color, int &max_width, int &max_height) const {
            TTF_SetFontSize(core::video::primary_font, font_size);
            SDL_Surface *surface = TTF_RenderText_Blended(core::video::primary_font, text.c_str(), color);
            if (!surface) return;
            SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!text_texture) {
                SDL_FreeSurface(surface);
                return;
            }
            const SDL_Rect rect = {5, max_height, surface->w, surface->h};
            max_width = std::max(max_width, surface->w + 10);
            max_height += surface->h;
            SDL_FreeSurface(surface);
            SDL_RenderCopy(renderer, text_texture, nullptr, &rect);
            SDL_DestroyTexture(text_texture);
        }

        void draw(const uint64_t &now) {
            const SDL_Color *selected_color = &normal_color;

            if (now > next_refresh) {
                next_refresh = now + core::system_freq * FRT_OVERLAY_UPDATE_INTERVAL / 1000; // refresh every 50ms
                // calculate
                const uint64_t ms_int_part = core::last_frame_time * 1000 / core::system_freq;
                const uint64_t ms_dec_part = core::last_frame_time * 1000 * 100 / core::system_freq % 100;
                uint64_t fps = 0;
                if (core::last_frame_time != 0) fps = core::system_freq / core::last_frame_time;
                // render text
                const std::string fps_text = std::to_string(fps) + "FPS";
                std::string frame_time_text = std::to_string(ms_int_part);
                if (ms_int_part < 10) {
                    frame_time_text += '.';
                    frame_time_text += std::to_string(ms_dec_part);
                }
                frame_time_text += "ms";
                // select color
                if (fps < 45) selected_color = &danger_color;
                else if (fps < 120) selected_color = &warn_color;
                // fill the texture with color
                SDL_SetRenderTarget(renderer, texture);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
                SDL_RenderClear(renderer);
                // draw the FPS text
                src_rect.w = 0;
                src_rect.h = 0;
                // render the text
                render_text(fps_text, 16, *selected_color, src_rect.w, src_rect.h);
                render_text(frame_time_text, 12, *selected_color, src_rect.w, src_rect.h);
                // dst_rect = get_overlay_render_position(
                //     core::video::RIGHT, core::video::BOTTOM,
                //     src_rect.w, src_rect.h, 10, 10
                // );
                dst_rect.w = src_rect.w;
                dst_rect.h = src_rect.h;
                dst_rect.x = core::video::render_rect.w - src_rect.w - 10;
                dst_rect.y = core::video::render_rect.h - src_rect.h - 10;
            }
            // draw the texture
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_RenderCopy(renderer, texture, &src_rect, &dst_rect);
        }

        ~FrameTimeOverlay() {
            if (texture) SDL_DestroyTexture(texture);
        }

    private:
        const SDL_Color normal_color = {0, 255, 0, 255};
        const SDL_Color warn_color = {255, 128, 0, 255};
        const SDL_Color danger_color = {255, 0, 0, 255};

        SDL_Rect src_rect { 0, 0, 100, 50};
        SDL_Rect dst_rect {};

        uint64_t next_refresh = 0;
        SDL_Renderer* renderer = nullptr;
        SDL_Texture* texture = nullptr;
    };
}
