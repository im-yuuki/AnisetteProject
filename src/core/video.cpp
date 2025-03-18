//
// Created by Yuuki on 09/03/2025.
//

#include "core.h"
#include <logging.h>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

const auto logger = anisette::logging::get("video");
const static SDL_DisplayMode *display_mode = nullptr;
static SDL_WindowFlags MAIN_WINDOW_FLAGS = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;

namespace anisette::core::video
{
    void process_frame(const uint64_t &counter) {
        static int r;
        r = (r + 1) % 256;
        SDL_SetRenderDrawColor(renderer, r, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    bool refresh_display_info() {
        const SDL_DisplayID display_id = SDL_GetDisplayForWindow(window);
        if (display_id == 0) {
            logger->warn("Failed to get display ID for window: {}", SDL_GetError());
            return false;
        }
        display_mode = SDL_GetDesktopDisplayMode(display_id);
        if (display_mode == nullptr) {
            logger->warn("Failed to get display mode of display {}: {}", display_id, SDL_GetError());
            return false;
        }
        logger->info("Display {}: {}x{}@{}Hz", display_id, display_mode->w, display_mode->h, display_mode->refresh_rate);
        return true;
    }

    bool splash() {
        logger->info("SDL_image version: {}.{}.{}", SDL_IMAGE_MAJOR_VERSION, SDL_IMAGE_MINOR_VERSION, SDL_IMAGE_MICRO_VERSION);
        logger->info("SDL_ttf version: {}.{}.{}", SDL_TTF_MAJOR_VERSION, SDL_TTF_MINOR_VERSION, SDL_TTF_MICRO_VERSION);
        if (!TTF_Init()) {
            logger->error("SDL_ttf init failed: {}", SDL_GetError());
            return false;
        }
        logger->debug("Initializing splash screen");
        if (!SDL_CreateWindowAndRenderer(
            "Anisette", 600, 200,
            SDL_WINDOW_BORDERLESS | SDL_WINDOW_TRANSPARENT | SDL_WINDOW_NOT_FOCUSABLE, &window, &renderer
            )) {
            logger->error("Initialize splash screen failed: {}", SDL_GetError());
            return false;
        }
        SDL_Texture *splash_logo = IMG_LoadTexture(renderer, "assets/logo.bmp");
        if (splash_logo == nullptr) {
            logger->error("Load splash logo failed: {}", SDL_GetError());
            return false;
        }
        const bool pipeline = SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0)
                            && SDL_RenderClear(renderer)
                            && SDL_RenderTexture(renderer, splash_logo, nullptr, nullptr)
                            && SDL_RenderPresent(renderer);
        if (!pipeline) {
            logger->error("Render failed: {}", SDL_GetError());
            return false;
        }
        SDL_DestroyTexture(splash_logo);
        return refresh_display_info();
    }

    bool init() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Delay(500); // just wait for splash screen to be closed
        logger->debug("Initializing main window");
        if (window = SDL_CreateWindow("Anisette", display_mode->w, display_mode->h, MAIN_WINDOW_FLAGS); window == nullptr) {
            logger->error("Initialize main window failed: {}", SDL_GetError());
            return false;
        } if (renderer = SDL_CreateRenderer(window, "opengl"); renderer == nullptr) {
            logger->error("Initialize renderer failed: {}", SDL_GetError());
            return false;
        }
        target_fps = static_cast<unsigned>(display_mode->refresh_rate);
        commit_update();
        const bool pipeline = SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255)
                            && SDL_RenderClear(renderer)
                            && SDL_RenderPresent(renderer);
        if (!pipeline) {
            logger->error("Render failed: {}", SDL_GetError());
            return false;
        }
        return true;
    }

    void cleanup() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
    }
}
