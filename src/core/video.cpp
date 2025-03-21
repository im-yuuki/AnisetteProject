//
// Created by Yuuki on 09/03/2025.
//

#include "core.h"
#include <logging.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

const auto logger = anisette::logging::get("video");

constexpr uint32_t SPLASH_WINDOW_INIT_FLAGS = SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN;
constexpr uint32_t SDL_IMAGE_INIT_FLAGS = IMG_INIT_PNG | IMG_INIT_JPG;
constexpr uint32_t RENDERER_INIT_FLAGS = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
constexpr uint32_t MAIN_WINDOW_FLAGS = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SHOWN;

static SDL_DisplayMode display_mode;
static SDL_Window *window = nullptr;

namespace anisette::core::video
{
    bool refresh_display_info() {
        const int display_id = SDL_GetWindowDisplayIndex(window);
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
        logger->info("SDL_image version: {}.{}.{}", SDL_IMAGE_MAJOR_VERSION, SDL_IMAGE_MINOR_VERSION, SDL_IMAGE_PATCHLEVEL);
        logger->info("SDL_ttf version: {}.{}.{}", SDL_TTF_MAJOR_VERSION, SDL_TTF_MINOR_VERSION, SDL_IMAGE_PATCHLEVEL);
        if (IMG_Init(SDL_IMAGE_INIT_FLAGS) != (SDL_IMAGE_INIT_FLAGS)) {
            logger->error("SDL_image init failed: {}", SDL_GetError());
            return false;
        }
        if (TTF_Init()) {
            logger->error("SDL_ttf init failed: {}", SDL_GetError());
            return false;
        }
        logger->debug("Initializing splash screen");
        if (SDL_CreateWindowAndRenderer(600, 200, SPLASH_WINDOW_INIT_FLAGS, &window, &renderer)) {
            logger->error("Initialize splash screen failed: {}", SDL_GetError());
            return false;
        }
        SDL_Texture *splash_logo = IMG_LoadTexture(renderer, "assets/logo.bmp");
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
        SDL_DestroyWindow(window);
        SDL_Delay(500); // just wait for splash screen to be closed
        logger->debug("Initializing main window");
        window = SDL_CreateWindow("Anisette", -1, -1, display_mode.w, display_mode.h, MAIN_WINDOW_FLAGS);
        if (window == nullptr) {
            logger->error("Initialize main window failed: {}", SDL_GetError());
            return false;
        } if (renderer = SDL_CreateRenderer(window, -1, RENDERER_INIT_FLAGS); renderer == nullptr) {
            logger->error("Initialize renderer failed: {}", SDL_GetError());
            return false;
        }
        target_fps = static_cast<unsigned>(display_mode.refresh_rate);
        apply_settings();
        if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) || SDL_RenderClear(renderer)) {
            logger->error("Render failed: {}", SDL_GetError());
            return false;
        }
        SDL_RenderPresent(renderer);
        return true;
    }

    void cleanup() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
    }
}
