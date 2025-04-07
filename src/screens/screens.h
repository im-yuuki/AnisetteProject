//
// Created by Yuuki on 22/03/2025.
//
#pragma once
#include "core.h"
#include "stage_channel.h"
#include "container.h"
#include <queue>

namespace anisette::screens {
    constexpr SDL_Color BTN_TEXT_COLOR        = {255, 255, 255, 255};
    constexpr SDL_Color BTN_BG_COLOR          = {0, 0, 0, 128};
    constexpr SDL_Color BTN_HOVER_COLOR       = {235, 0, 85, 255};
    constexpr SDL_Color BTN_DISABLED_COLOR    = {0, 0, 0, 255};
    constexpr SDL_Color BTN_TRANSPARENT_COLOR = {0, 0, 0, 0};

    class StageScreen final : public core::abstract::Screen {
        explicit StageScreen(SDL_Renderer *renderer);
        ~StageScreen() override;

        void on_event(const uint64_t &now, const SDL_Event &event) override;
        void update(const uint64_t &now) override;
        void on_focus(const uint64_t &now) override;
    };

    class LibraryScreen final : public core::abstract::Screen {
    public:
        explicit LibraryScreen(SDL_Renderer *renderer);
        ~LibraryScreen() override;

        void on_event(const uint64_t &now, const SDL_Event &event) override;
        void on_focus(const uint64_t &now) override;
        void update(const uint64_t &now) override;

    private:
        components::IconButton    *back_btn;
        components::IconButton    *arr_left_btn;
        components::IconButton    *arr_right_btn;
        components::TextButton    *play_btn;
        components::HorizontalBox *top_bar;
        components::HorizontalBox *body;
        components::HorizontalBox *bottom_bar;
        components::VerticalBox    main_layout {0, 2};

        const int selected_song_id = -1;
    };

    class MenuScreen final : public core::abstract::Screen {
    public:
        explicit MenuScreen(SDL_Renderer *renderer);
        ~MenuScreen() override = default;

        void load_async();
        bool is_load_async_finished();

        void on_click(const uint64_t &now, int x, int y) const;
        void on_event(const uint64_t &now, const SDL_Event &event) override;
        void update(const uint64_t &now) override;
        void on_focus(const uint64_t &now) override;
        void play_random_music() const;

    private:
        // menu components
        components::TextButton  *play_btn,
                                *settings_btn,
                                *quit_btn;
        // music components
        components::IconButton  *music_play_btn,
                                *music_pause_btn,
                                *music_stop_btn,
                                *music_next_btn,
                                *music_prev_btn;
        components::Text        *now_playing_text;
        components::ItemWrapper *music_play_btn_wrapper,
                                *music_pause_btn_wrapper;
        // volume overlay components
        components::ItemWrapper *volume_mute_icon,
                                *volume_low_icon,
                                *volume_high_icon;
        components::Text        *volume_text;
        components::ProgressBar *volume_bar;
        components::HorizontalBox *volume_overlay;
        uint64_t volume_overlay_hide_time = 0;
        // main grid
        components::Grid grid {2};

        uint64_t action_start_time = 0;
        std::queue<std::function<bool(const uint64_t &now)>> action_hook;
        std::atomic_bool load_async_finshed = false;
        std::vector<std::string> default_backgrounds;
    };

    class SplashScreen final : public core::abstract::Screen {
    public:
        explicit SplashScreen(SDL_Renderer *renderer);
        ~SplashScreen() override;

        void on_event(const uint64_t &now, const SDL_Event &event) override {}
        void update(const uint64_t &now) override;
        void on_focus(const uint64_t &now) override {};

    private:
        const uint64_t fade_duration = core::system_freq; // 1 second
        uint64_t action_start_time = 0;
        std::queue<std::function<bool(const uint64_t &now)>> action_hook;

        MenuScreen* menu_screen;
        SDL_Texture* logo;
        SDL_Rect logo_rect {};
    };

    inline void load() {
        core::register_first_screen([&](SDL_Renderer *renderer) {
            return new SplashScreen(renderer);
        });
    }
}