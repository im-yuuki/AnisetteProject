//
// Created by Yuuki on 22/03/2025.
//
#include "scene_register.h"
#include <discord.h>
#include <scene.h>
#include <SDL2/SDL_render.h>

namespace anisette::screens {
    MainMenu::MainMenu(SDL_Renderer *renderer) : Scene(renderer) {}
    MainMenu::~MainMenu() {}

    void MainMenu::on_focus() {
        utils::discord::set_in_main_menu();
    }

    void MainMenu::on_leave() {

    }

    bool MainMenu::handle_event(const SDL_Event &event) {
        return false;
    }

    void MainMenu::handle_frame(const uint64_t &start_frame) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
}