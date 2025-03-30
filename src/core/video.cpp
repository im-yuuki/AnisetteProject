//
// Created by Yuuki on 09/03/2025.
//

#include "core.h"
#include "config.h"
#include "utils/logging.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

const auto logger = anisette::logging::get("video");

constexpr uint32_t RENDERER_INIT_FLAGS = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
constexpr uint32_t WINDOW_INIT_FLAGS = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

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

    bool init() {
        logger->debug("Initializing main window");
        auto flags = WINDOW_INIT_FLAGS;
        if (config::display_mode == config::EXCLUSIVE) flags |= SDL_WINDOW_FULLSCREEN;
        else if (config::display_mode == config::BORDERLESS) flags |= SDL_WINDOW_BORDERLESS;
        _window = SDL_CreateWindow(
            "Anisette",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            config::render_width, config::render_height, flags
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
        return refresh_display_info();
    }

    void cleanup() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(_window);
    }
}
