//
// Created by Yuuki on 21/03/2025.
//

#pragma once
#include <SDL2/SDL_events.h>
#include <SDL_render.h>

namespace anisette::core::abstract {
    class FrameHandler {
    public:
        virtual ~FrameHandler() = default;
        virtual void on_event(const uint64_t &start_frame, const SDL_Event &event) = 0;
        virtual void update(const uint64_t &start_frame) = 0;
    };
}
