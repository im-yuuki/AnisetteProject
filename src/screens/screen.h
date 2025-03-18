//
// Created by Yuuki on 18/03/2025.
//
#pragma once
#include <SDL3/SDL_render.h>

namespace anisette::screens
{
    class Screen
    {
    public:
        virtual void focus_enter() = 0;
        virtual void focus_leave() = 0;
    private:
        SDL_Renderer *renderer;
    };
}
