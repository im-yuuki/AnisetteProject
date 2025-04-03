//
// Created by Yuuki on 31/03/2025.
//
#include <SDL_image.h>
#include "core/core.h"
#include "register.h"
#include "utils/common.h"
#include "utils/logging.h"

const auto logger = anisette::logging::get("splash");

namespace anisette::screens {
    SplashScreen::SplashScreen(SDL_Renderer *renderer) : renderer(renderer) {
        logo_rect = get_overlay_render_position(core::video::CENTER, core::video::MIDDLE, 600, 200, 0, 0);
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
        action_hook.emplace([this](const uint64_t &now) {
            const auto delta = now > action_start_time ? now - action_start_time : 0;
            const auto alpha = 255 * delta / fade_duration;
            if (alpha > 255) {
                SDL_SetTextureAlphaMod(logo, 255);
                return true;
            }
            SDL_SetTextureAlphaMod(logo, alpha);
            return false;
        });
        // hook check if menu screen is loaded
        action_hook.emplace([this](const uint64_t &now) {
            return menu_screen->is_load_async_finished();
        });
        // hook fade out action + switch to menu screen
        action_hook.emplace([this](const uint64_t &now) {
            const auto delta = now > action_start_time ? now - action_start_time : 0;
            const auto alpha = 255 * delta / fade_duration;
            if (alpha > 255) {
                SDL_DestroyTexture(logo);
                logo = nullptr;
                // switch to menu screen
                core::open(menu_screen);
                return true;
            }
            SDL_SetTextureAlphaMod(logo, 255 - alpha);
            return false;
        });
        // hook quit action (if stack go back to this screen)
        action_hook.emplace([this](const uint64_t &now) {
            core::request_stop();
            return false;
        });
    };

    void SplashScreen::update(const uint64_t &now) {
        // Render the scene
        if (!logo) return;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, logo, nullptr, &logo_rect);

        // run action hooks
        if (!action_hook.empty()) if (action_hook.front()(now)) {
            action_hook.pop();
            action_start_time = now;
        }
    }

    SplashScreen::~SplashScreen() {
        SDL_DestroyTexture(logo);
    };
}