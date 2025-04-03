//
// Created by Yuuki on 03/04/2025.
//
#pragma once
#include "core/config.h"
#include "core/static.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <atomic>
#include <cstdint>
#include <string>

#define BACKGROUND_PARALLAX_RANGE_PERCENT 95
#define BACKGROUND_SWAP_DURATION_MS 5000

namespace anisette::components {
    class Background {
    public:
        explicit Background(SDL_Renderer *renderer) : renderer(renderer) {
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, core::config::render_width, core::config::render_height);
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            new_texture = nullptr;
            start_time = 0;
        }

        std::atomic_bool enable_parallax = false;

        void load(const std::string &path, const uint64_t &now) {
            if (new_texture) SDL_DestroyTexture(new_texture);
            if (path.empty()) {
                new_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, core::config::render_width, core::config::render_height);
            };
            if (path != current_img_path) {
                current_img_path = path;
                new_texture = IMG_LoadTexture(renderer, path.c_str());
            }
            if (new_texture) {
                SDL_SetTextureBlendMode(new_texture, SDL_BLENDMODE_BLEND);
                SDL_SetTextureAlphaMod(new_texture, 0);
                int width, height;
                SDL_QueryTexture(new_texture, nullptr, nullptr, &width, &height);
                const auto screen_ratio = static_cast<double>(core::config::render_width) / core::config::render_height;
                const auto img_ratio = static_cast<double>(width) / height;
                // calculate background rect
                if (screen_ratio > img_ratio) {
                    // stretch to fit width
                    bg_rect.w = width;
                    bg_rect.h = width / screen_ratio;
                    bg_rect.x = 0;
                    bg_rect.y = abs(height - bg_rect.h) / 2;
                } else {
                    // stretch to fit height
                    bg_rect.w = height * screen_ratio;
                    bg_rect.h = height;
                    bg_rect.x = abs(width - bg_rect.w) / 2;
                    bg_rect.y = 0;
                }
                start_time = SDL_GetPerformanceCounter();
            }
        }

        void draw(const uint64_t& now) {
            if (!texture) return;
            if (new_texture) {
                // calculate alpha
                const uint64_t delta = now > start_time ? now - start_time : 0;
                auto alpha = 255 * delta * 1000 / BACKGROUND_SWAP_DURATION_MS / core::system_freq;
                if (alpha > 255) alpha = 255;
                SDL_SetTextureAlphaMod(new_texture, alpha);
                // render to buffer
                SDL_SetRenderTarget(renderer, texture);
                SDL_RenderCopy(renderer, new_texture, &bg_rect, nullptr);
                if (alpha == 255) {
                    SDL_DestroyTexture(new_texture);
                    new_texture = nullptr;
                }
            }
            SDL_Rect draw_rect = {0, 0, core::config::render_width, core::config::render_height};
            if (enable_parallax) {
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                // calculate parallax
                draw_rect.w = draw_rect.w * BACKGROUND_PARALLAX_RANGE_PERCENT / 100;
                draw_rect.h = draw_rect.h * BACKGROUND_PARALLAX_RANGE_PERCENT / 100;
                draw_rect.x = (core::config::render_width - draw_rect.w) * mouse_x / core::config::render_width;
                draw_rect.y = (core::config::render_height - draw_rect.h) * mouse_y / core::config::render_height;
            }
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, &draw_rect, nullptr);
        }

        ~Background() {
            SDL_DestroyTexture(texture);
            SDL_DestroyTexture(new_texture);
        }

    private:
        SDL_Renderer *renderer;
        SDL_Rect bg_rect {0, 0, 0, 0};
        std::string current_img_path;
        SDL_Texture *new_texture;
        SDL_Texture *texture;
        uint64_t start_time;
    };
}