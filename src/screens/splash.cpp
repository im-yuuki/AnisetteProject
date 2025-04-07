//
// Created by Yuuki on 31/03/2025.
//
#include <SDL2/SDL_image.h>
#include "core.h"
#include "screens.h"
#include "common.h"
#include "logging.h"

const auto logger = anisette::logging::get("splash");

namespace anisette::screens {
    SplashScreen::SplashScreen(SDL_Renderer *renderer) {
        this->renderer = renderer;
        logo_rect.w = 600;
        logo_rect.h = 200;
        logo_rect.x = (core::video::render_rect.w - logo_rect.w) / 2;
        logo_rect.y = (core::video::render_rect.h - logo_rect.h) / 2;
        logo = IMG_LoadTexture(renderer, "assets/logo.png");
        if (!logo) {
            logger->error("Failed to load logo texture: {}", SDL_GetError());
            return;
        }
        SDL_SetTextureBlendMode(logo, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(logo, 0);
        // load menu screen
        menu_screen = new MenuScreen(renderer);
        menu_screen->load_async();
        // hook fade in action
        action_start_time = SDL_GetPerformanceCounter();
    };

    void SplashScreen::update(const uint64_t &now) {
        // run action hooks
        if (!action_hook.empty()) if (action_hook.front()(now)) {
            action_hook.pop();
            action_start_time = now;
        }
        // Render the scene
        if (!logo) return;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, logo, nullptr, &logo_rect);
    }

    SplashScreen::~SplashScreen() {
        SDL_DestroyTexture(logo);
    };

    void SplashScreen::on_focus(const uint64_t &now) {
        if (hook_finished) {
            core::request_stop();
            return;
        }
        action_start_time = now;
        // hook check if beatmap loader is ready
        action_hook.emplace([this](const uint64_t &action_now) {
            return core::beatmap_loader->is_scan_finished();
        });
        // hook check if menu screen is loaded
        action_hook.emplace([this](const uint64_t &action_now) {
            return menu_screen->is_load_async_finished();
        });
        // hook fade out action + switch to menu screen
        action_hook.emplace([this](const uint64_t &action_now) {
            const auto delta = action_now > action_start_time ? action_now - action_start_time : 0;
            const auto alpha = 255 * delta / fade_duration;
            if (alpha > 255) {
                SDL_DestroyTexture(logo);
                logo = nullptr;
                hook_finished = true;
                // switch to menu screen
                core::open(menu_screen);
                return true;
            }
            SDL_SetTextureAlphaMod(logo, 255 - alpha);
            return false;
        });
    }

}