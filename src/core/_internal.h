//
// Created by Yuuki on 31/03/2025.
//
#pragma once
#include "core.h"
#include "utils/common.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <atomic>
#include <cstdint>
#include <string>
#include <cassert>

#define FRT_OVERLAY_FONT_FILE "assets/fonts/Roboto-Bold.ttf"

namespace anisette::core
{
    extern uint64_t last_frame_time;
    extern std::atomic_bool stop_requested;
    extern uint64_t target_frame_time;

    extern bool init();
    extern void cleanup();
    extern void _main_loop();

    class FrameTimeOverlay {
    public:
        FrameTimeOverlay(SDL_Renderer *renderer) : renderer(renderer) {
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 100, 100);
            assert(texture);
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            fps_font = TTF_OpenFont(FRT_OVERLAY_FONT_FILE, 16);
            frame_time_font = TTF_OpenFont(FRT_OVERLAY_FONT_FILE, 12);
        };

        void render_text(const std::string &text, TTF_Font* font, const SDL_Color &color, int &max_width, int &max_height) const {
            SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
            assert(surface);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            assert(texture);
            const SDL_Rect rect = {5, max_height, surface->w, surface->h};
            max_width = std::max(max_width, surface->w + 10);
            max_height += surface->h;
            SDL_FreeSurface(surface);
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
            SDL_DestroyTexture(texture);
        }

        void draw(const uint64_t &now) {
            std::string fps_text, frame_time_text;
            uint64_t ms_int_part, ms_dec_part, fps;
            const SDL_Color *selected_color = &color;
            // render directly from the buffer
            if (now < next_refresh) goto render;
            next_refresh = now + utils::system_freq / 20; // refresh every 50ms
            // calculate
            ms_int_part = last_frame_time * 1000 / utils::system_freq;
            ms_dec_part = last_frame_time * 1000 * 100 / utils::system_freq % 100;
            if (last_frame_time == 0) fps = 0;
            else fps = utils::system_freq / last_frame_time;
            // render text
            fps_text = std::to_string(fps) + "FPS";
            frame_time_text = std::to_string(ms_int_part);
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
            render_text(fps_text, fps_font, *selected_color, src_rect.w, src_rect.h);
            render_text(frame_time_text, frame_time_font, *selected_color, src_rect.w, src_rect.h);
            dst_rect = get_overlay_render_position(
                core::video::RIGHT, core::video::BOTTOM,
                src_rect.w, src_rect.h, 10, 10
            );

            // draw the texture
            render:
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_RenderCopy(renderer, texture, &src_rect, &dst_rect);
        }

        ~FrameTimeOverlay() {
            if (texture) SDL_DestroyTexture(texture);
            if (frame_time_font) TTF_CloseFont(frame_time_font);
            if (fps_font) TTF_CloseFont(fps_font);
        }
    private:
        const SDL_Color color = {0, 255, 0, 255};
        const SDL_Color warn_color = {255, 128, 0, 255};
        const SDL_Color danger_color = {255, 0, 0, 255};

        SDL_Rect src_rect { 0, 0, 100, 50};
        SDL_Rect dst_rect = get_overlay_render_position(
            core::video::RIGHT, core::video::BOTTOM,
            src_rect.w, src_rect.h, 10, 10
            );

        uint64_t next_refresh = 0;
        SDL_Renderer* renderer = nullptr;
        SDL_Texture* texture = nullptr;
        TTF_Font* frame_time_font = nullptr;
        TTF_Font* fps_font = nullptr;
    };

    extern FrameTimeOverlay *frame_time_overlay;
} // namespace anisette::core

namespace anisette::core::video
{
    extern SDL_Window *window;
    extern SDL_Renderer *renderer;
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
} // namespace anisette::core::audio
