//
// Created by Yuuki on 22/03/2025.
//
#pragma once
#include "core/abs.h"
#include "core/core.h"

#include <SDL_mixer.h>
#include <utils/common.h>

namespace anisette::screens {
    class SplashScreen final : public core::abstract::Screen {
    public:
        explicit SplashScreen(SDL_Renderer *renderer);
        void on_event(const uint64_t &now, const SDL_Event &event) override;
        ~SplashScreen() override;
        void update(const uint64_t &now) override;
    private:
        SDL_Renderer* renderer;
        SDL_Texture* logo;
        SDL_Rect logo_rect {};
        uint64_t start_time = 0;
        bool fade_in_ended = false;
        const uint64_t fade_duration = utils::system_freq * 15 / 10; // 1.5 seconds
    };

    class MenuScreen final : public core::abstract::Screen {
    public:
        explicit MenuScreen(SDL_Renderer *renderer);
        void on_event(const uint64_t &now, const SDL_Event &event) override;
        void update(const uint64_t &now) override;
        ~MenuScreen() override;
    private:
        const uint8_t parallax_range = 100;

        Mix_Chunk* click_sound = nullptr;
        SDL_Texture* background = nullptr;
        SDL_Renderer* renderer;
        SDL_DisplayMode* display_mode = nullptr;
        SDL_Rect bg_src_rect {0, 0, 0, 0};
    };

    inline void load() {
        core::register_first_screen([&](SDL_Renderer *renderer) {
            return new SplashScreen(renderer);
        });
    }
}