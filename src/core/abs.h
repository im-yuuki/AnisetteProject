//
// Created by Yuuki on 21/03/2025.
//

#pragma once
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_events.h>

namespace anisette::core::abstract {
    class Scene {
    public:
        virtual void post_load();
        virtual void pre_unload();
        virtual void on_focus();
        virtual void on_leave();
        virtual bool handle_event(const SDL_Event &event);
        virtual void handle_frame(const uint64_t &counter);
    private:
        SDL_Renderer *renderer;
    };
}