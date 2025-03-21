//
// Created by Yuuki on 22/03/2025.
//
#pragma once
#include <scene.h>
#include <SDL2/SDL_render.h>

namespace anisette::screens {
    class MainMenu final : public core::Scene {
    public:
        explicit MainMenu(SDL_Renderer *renderer);
        ~MainMenu() override;
        void on_focus() override;
        void on_leave() override;
        bool handle_event(const SDL_Event &event) override;
        void handle_frame(const uint64_t &start_frame) override;
    };

    inline void register_scene() {
        const auto instance = core::SceneFactory::instance();
        instance->register_scene("main_menu", [](SDL_Renderer *renderer) { return new MainMenu(renderer); });
    }
}