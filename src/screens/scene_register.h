//
// Created by Yuuki on 22/03/2025.
//
#pragma once
#include <core.h>
#include <SDL2/SDL_render.h>

namespace anisette::screens {
    class MainMenu final : public core::abstract::FrameHandler {
    public:
        bool handle_event(const SDL_Event &event) override;
        void handle_frame(const uint64_t &start_frame) override;
    };
}