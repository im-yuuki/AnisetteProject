//
// Created by Yuuki on 22/03/2025.
//
#pragma once
#include "core/abs.h"
#include <SDL2/SDL_render.h>

namespace anisette::screens {
    class Screen : public core::abstract::FrameHandler {
    public:
        Screen();
        bool handle_event(const uint64_t &start_frame, const SDL_Event &event) override;
        void handle_frame(const uint64_t &start_frame) override;
    };
}