//
// Created by Yuuki on 09/03/2025.
//

#include "core.h"
#include <logging.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

const auto logger = anisette::logging::get("video");

constexpr uint32_t SPLASH_WINDOW_INIT_FLAGS = SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN;
constexpr uint32_t RENDERER_INIT_FLAGS = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
constexpr uint32_t MAIN_WINDOW_FLAGS = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SHOWN;

namespace anisette::core::video
{
    static SDL_Window *_window = nullptr;

    bool refresh_display_info() {
        const int display_id = SDL_GetWindowDisplayIndex(_window);
        if (display_id < 0) {
            logger->warn("Failed to get display ID for window: {}", SDL_GetError());
            return false;
        }
        if (SDL_GetDesktopDisplayMode(display_id, &display_mode)) {
            logger->warn("Failed to get display mode of display {}: {}", display_id, SDL_GetError());
            return false;
        }
        logger->info("Display {}: {}x{}@{}Hz", display_id, display_mode.w, display_mode.h, display_mode.refresh_rate);
        return true;
    }

    bool splash() {
        logger->debug("Initializing splash screen");
        if (SDL_CreateWindowAndRenderer(600, 200, SPLASH_WINDOW_INIT_FLAGS, &_window, &renderer)) {
            logger->error("Initialize splash screen failed: {}", SDL_GetError());
            return false;
        }
        SDL_Texture *splash_logo = IMG_LoadTexture(renderer, "assets/logo.png");
        if (splash_logo == nullptr) {
            logger->error("Load splash logo failed: {}", SDL_GetError());
            return false;
        }
        const bool flow = SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0)
                        || SDL_RenderClear(renderer)
                        || SDL_RenderCopy(renderer, splash_logo, nullptr, nullptr);
        if (flow) {
            logger->error("Render failed: {}", SDL_GetError());
            return false;
        }
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(splash_logo);
        return refresh_display_info();
    }

    bool init() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(_window);
        SDL_Delay(500); // just wait for splash screen to be closed
        logger->debug("Initializing main window");
        _window = SDL_CreateWindow(
            "Anisette",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            display_mode.w, display_mode.h,
            MAIN_WINDOW_FLAGS
            );
        if (_window == nullptr) {
            logger->error("Initialize main window failed: {}", SDL_GetError());
            return false;
        } if (renderer = SDL_CreateRenderer(_window, -1, RENDERER_INIT_FLAGS); renderer == nullptr) {
            logger->error("Initialize renderer failed: {}", SDL_GetError());
            return false;
        }
        if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) || SDL_RenderClear(renderer)) {
            logger->error("Render failed: {}", SDL_GetError());
            return false;
        }
        SDL_RenderPresent(renderer);
        return true;
    }

    void cleanup() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(_window);
    }
}
