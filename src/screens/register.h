//
// Created by Yuuki on 22/03/2025.
//
#pragma once
#include "core/abs.h"
#include "core/core.h"

#include <SDL_mixer.h>
#include <utils/common.h>
#include <queue>

namespace anisette::screens {
    class MenuScreen final : public core::abstract::Screen {
    public:
        explicit MenuScreen(SDL_Renderer *renderer);
        ~MenuScreen() override;

        void load_async();
        bool is_load_async_finished() { return load_async_finished; }

        void on_event(const uint64_t &now, const SDL_Event &event) override;
        void update(const uint64_t &now) override;
        void on_back(const uint64_t &now) override;

    private:
        const uint8_t parallax_range = 100;
        std::atomic_bool load_async_finished = false;

        Mix_Chunk* click_sound = nullptr;
        SDL_Texture* background = nullptr;
        SDL_Renderer* renderer;
        SDL_DisplayMode* display_mode = nullptr;
        SDL_Rect bg_src_rect {0, 0, 0, 0};
    };

    class SplashScreen final : public core::abstract::Screen {
    public:
        explicit SplashScreen(SDL_Renderer *renderer);
        void on_event(const uint64_t &now, const SDL_Event &event) override {}
        ~SplashScreen() override;
        void update(const uint64_t &now) override;
        void on_back(const uint64_t &now) override {};

    private:
        const uint64_t fade_duration = utils::system_freq; // 1s, multiply it with duration in seconds you want

        uint64_t action_start_time = 0;
        std::queue<std::function<bool(const uint64_t &now)>> action_hook;

        MenuScreen* menu_screen;
        SDL_Renderer* renderer;
        SDL_Texture* logo;
        SDL_Rect logo_rect {};
    };

    inline void load() {
        core::register_first_screen([&](SDL_Renderer *renderer) {
            return new SplashScreen(renderer);
        });
    }
}