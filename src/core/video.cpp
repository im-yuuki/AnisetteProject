//
// Created by Yuuki on 09/03/2025.
//

#include "core.h"
#include <logging.h>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>

#ifndef RENDER_DRIVER_LIST
#define RENDER_DRIVER_LIST "opengl,opengles2"
#endif

const auto logger = anisette::logging::get("video");
constexpr SDL_WindowFlags WINDOW_INIT_FLAGS = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;

static int r = 255, g = 127, b = 0;

namespace anisette::core::video {
    void process_frame() {
        r = (r + 1) % 256;
        g = (g + 1) % 256;
        b = (b + 1) % 256;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    void configure_window() {
        const SDL_DisplayID display_id = SDL_GetDisplayForWindow(window);
        if (display_id == 0) {
            logger->warn("Failed to get display ID for window");
            return;
        }
        const SDL_DisplayMode *display_mode = SDL_GetDesktopDisplayMode(display_id);
        logger->info("Using display {}: {}x{}@{}Hz", display_id, display_mode->w, display_mode->h, display_mode->refresh_rate);
        SDL_SetWindowFullscreenMode(window, display_mode);
        if (!SDL_SyncWindow(window)) {
            logger->warn("Failed to configure window: {}", SDL_GetError());
        }
    }

    bool init() {
        logger->info("SDL_image version: {}.{}.{}",
            SDL_IMAGE_MAJOR_VERSION,
            SDL_IMAGE_MINOR_VERSION,
            SDL_IMAGE_MICRO_VERSION
            );
        logger->debug("Initialize window with flags {}", WINDOW_INIT_FLAGS);
        window = SDL_CreateWindow(
            "Anisette",
            1920, 1080,
            WINDOW_INIT_FLAGS
            );
        if (window == nullptr) {
            logger->error("Initialize window failed: {}", SDL_GetError());
            return false;
        }
        configure_window();
        logger->debug("Creating renderer");
        renderer = SDL_CreateRenderer(window, RENDER_DRIVER_LIST);
        if (renderer == nullptr) {
            logger->error("Create renderer failed: {}", SDL_GetError());
            return false;
        }
        return true;
    }

    void cleanup() {
        logger->debug("Destroying window");
        SDL_DestroyWindow(window);
    }
}