//
// Created by Yuuki on 01/04/2025.
//
#pragma once
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "config.h"

#include <cassert>

namespace anisette::components
{
    class FrameTimeOverlay {
    public:
        FrameTimeOverlay(SDL_Renderer *renderer, const uint64_t *last_frame_time) : last_frame_time(last_frame_time), renderer(renderer) {
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, overlay_rect.w, overlay_rect.h);
            fps_font = TTF_OpenFont("assets/Roboto-Regular.ttf", 14);
            frame_time_font = TTF_OpenFont("assets/Roboto-Regular.ttf", 12);
        };

        void render_text(const std::string &text, TTF_Font* font, const SDL_Color &color, int &margin_top) const {
            SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
            assert(surface);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            assert(texture);
            const SDL_Rect rect = {5, margin_top, surface->w, surface->h};
            margin_top += surface->h;
            SDL_FreeSurface(surface);
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
            SDL_DestroyTexture(texture);
        }

        void draw() const {
            // calculate
            const auto frame_time = last_frame_time != nullptr ? *last_frame_time : 0;
            // render text
            const std::string frame_time_text = std::to_string(frame_time * 1000 / sys_freq) + "ms";
            uint64_t fps;
            if (frame_time == 0) fps = 0;
            else fps = sys_freq / frame_time;
            // select color
            const SDL_Color* selected_color = &color;
            if (fps < 45) selected_color = &danger_color;
            else if (fps < 120) selected_color = &warn_color;

            const std::string fps_text = std::to_string(fps) + "FPS";
            // fill the texture with color
            SDL_SetRenderTarget(renderer, texture);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 10);
            SDL_RenderClear(renderer);
            // draw the FPS text
            int margin_top = 0;
            render_text(fps_text, fps_font, *selected_color, margin_top);
            render_text(frame_time_text, frame_time_font, *selected_color, margin_top);
            // draw the texture
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_RenderCopy(renderer, texture, nullptr, &overlay_rect);
        }

        ~FrameTimeOverlay() {
            if (texture) SDL_DestroyTexture(texture);
            if (frame_time_font) TTF_CloseFont(frame_time_font);
            if (fps_font) TTF_CloseFont(fps_font);
        }
    private:
        const uint64_t sys_freq = SDL_GetPerformanceFrequency();
        const SDL_Color color = {0, 255, 0, 255};
        const SDL_Color warn_color = {255, 128, 0, 255};
        const SDL_Color danger_color = {255, 0, 0, 255};
        SDL_Rect overlay_rect = get_overlay_render_position(core::video::RIGHT, core::video::BOTTOM, 80, 40, 10, 10);

        const uint64_t* last_frame_time = nullptr;

        SDL_Renderer* renderer = nullptr;
        SDL_Texture* texture = nullptr;
        TTF_Font* frame_time_font = nullptr;
        TTF_Font* fps_font = nullptr;
    };
}
