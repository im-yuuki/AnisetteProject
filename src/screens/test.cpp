//
// Created by Yuuki on 31/03/2025.
//
#include "register.h"
#include "core/config.h"

namespace anisette::screens {
    TestScreen::TestScreen(SDL_Renderer* renderer) : renderer(renderer) {};

    void TestScreen::update(const uint64_t &start_frame) {
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
    }

    void TestScreen::on_event(const uint64_t &start_frame, const SDL_Event &event) {
    }

    TestScreen::~TestScreen() = default;
}
