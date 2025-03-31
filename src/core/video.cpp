//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include "_internal.h"
#include "config.h"
#include "utils/logging.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

const auto logger = anisette::logging::get("video");
namespace anisette::core::video
{
    bool init() {
        // Initialize
        logger->debug("Initializing main window");
        uint32_t flags = 0;
        if (config::display_mode == config::EXCLUSIVE) flags |= SDL_WINDOW_FULLSCREEN;
        else if (config::display_mode == config::BORDERLESS) flags |= SDL_WINDOW_BORDERLESS;
        window = SDL_CreateWindow(
            "Anisette",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            config::render_width, config::render_height, flags
            );
        if (!window) {
            logger->error("Initialize main window failed: {}", SDL_GetError());
            return false;
        }
        if (!refresh_display_info()) return false;
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        if (!renderer) {
            logger->error("Initialize main window failed: {}", SDL_GetError());
            return false;
        }
        return true;
    }

    bool refresh_display_info() {
        if (SDL_GetDesktopDisplayMode(SDL_GetWindowDisplayIndex(window), &display_mode)) {
            logger->error("Failed to query display information: {}", SDL_GetError());
            return false;
        }
        logger->info("Display info: {}x{}@{}Hz", display_mode.w, display_mode.h, display_mode.refresh_rate);
        return true;
    }

    void cleanup() {
        SDL_DestroyWindow(window);
    }
}
