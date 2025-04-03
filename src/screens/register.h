//
// Created by Yuuki on 22/03/2025.
//
#pragma once
#include "core/abs.h"
#include "core/core.h"
#include "utils/common.h"

#include <SDL_mixer.h>
#include <queue>

namespace anisette::screens {
    class StageScreen final : public core::abstract::Screen {
        void on_event(const uint64_t &now, const SDL_Event &event) override;
        void update(const uint64_t &now) override;
        void on_focus(const uint64_t &now) override;
    };

    class MenuScreen final : public core::abstract::Screen {
    public:
        explicit MenuScreen(SDL_Renderer *renderer);
        ~MenuScreen() override;

        void load_async();
        bool is_load_async_finished();

        void on_click(const uint64_t &now, const int x, const int y);
        void on_event(const uint64_t &now, const SDL_Event &event) override;
        void update(const uint64_t &now) override;
        void on_focus(const uint64_t &now) override;

    private:
        uint64_t action_start_time = 0;
        std::queue<std::function<bool(const uint64_t &now)>> action_hook;

        std::atomic_bool load_async_finished = false;

        Mix_Chunk* click_sound = nullptr;
        SDL_Renderer* renderer;

        std::vector<std::string> default_backgrounds;
    };

    class SplashScreen final : public core::abstract::Screen {
    public:
        explicit SplashScreen(SDL_Renderer *renderer);
        void on_event(const uint64_t &now, const SDL_Event &event) override {}
        ~SplashScreen() override;
        void update(const uint64_t &now) override;
        void on_focus(const uint64_t &now) override {};

    private:
        const uint64_t fade_duration = utils::system_freq; // 1 second
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