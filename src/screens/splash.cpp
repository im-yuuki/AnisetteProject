//
// Created by Yuuki on 31/03/2025.
//
#include <SDL_image.h>
#include <cassert>
#include "core/core.h"
#include "register.h"
#include "utils/common.h"

namespace anisette::screens {
    SplashScreen::SplashScreen(SDL_Renderer *renderer) : renderer(renderer) {
        logo_rect = get_overlay_render_position(core::video::CENTER, core::video::MIDDLE, 600, 200, 0, 0);
        logo = IMG_LoadTexture(renderer, "assets/logo.png");
        assert(logo);
        SDL_SetTextureBlendMode(logo, SDL_BLENDMODE_BLEND);
        // Start with fully transparent texture
        SDL_SetTextureAlphaMod(logo, 0);
        start_time = SDL_GetPerformanceCounter();
    };

    void SplashScreen::update(const uint64_t &now) {
        if (!fade_in_ended) {
            const int alpha = utils::calc_fade_alpha(false, start_time, now, fade_duration);
            SDL_SetTextureAlphaMod(logo, alpha);
            if (alpha == 255) fade_in_ended = true;
        }

        // Render the scene
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, logo, nullptr, &logo_rect);

        if (start_time + utils::system_freq * 3 < now) {
            core::open(new MenuScreen(renderer));
        }
    }

    void SplashScreen::on_event(const uint64_t &now, const SDL_Event &event) {
    }

    SplashScreen::~SplashScreen() {
        SDL_DestroyTexture(logo);
    };
}