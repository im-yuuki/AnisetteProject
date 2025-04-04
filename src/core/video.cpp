//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include "internal.h"
#include "config.h"
#include "logging.h"
#include <SDL2/SDL_render.h>

#define PRIMARY_FONT_PATH "assets/fonts/Roboto-Bold.ttf"
#define SECONDARY_FONT_PATH "assets/fonts/Roboto-Regular.ttf"

const auto logger = anisette::logging::get("video");
namespace anisette::core::video
{
    SDL_Rect render_rect {0, 0, 0, 0};
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_DisplayMode display_mode {};

    TTF_Font *primary_font = nullptr;
    TTF_Font *secondary_font = nullptr;

    bool init() {
        // Initialize
        render_rect.w = config::render_width;
        render_rect.h = config::render_height;

        logger->debug("Initializing main window");
        uint32_t flags = SDL_WINDOW_OPENGL;
        if (config::display_mode == config::WINDOWED) {
            flags |= SDL_WINDOW_RESIZABLE;
        } else if (config::display_mode == config::BORDERLESS) {
            flags |= SDL_WINDOW_BORDERLESS;
        }
        logger->debug("Initializing window with flags: {}", flags);
        window = SDL_CreateWindow(
            "Anisette",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            render_rect.w, render_rect.h, flags
            );
        if (!window) {
            logger->error("Initialize main window failed: {}", SDL_GetError());
            return false;
        }
        logger->debug("Initializing renderer");
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        if (!renderer) {
            logger->error("Initialize renderer failed: {}", SDL_GetError());
            return false;
        }
        if (SDL_GetDesktopDisplayMode(SDL_GetWindowDisplayIndex(window), &display_mode)) {
            logger->error("Fetch display mode failed: {}", SDL_GetError());
            return false;
        }
        logger->info("Display info: {}x{}@{}Hz", display_mode.w, display_mode.h, display_mode.refresh_rate);

        if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND)) {
            logger->error("Set renderer blend mode failed: {}", SDL_GetError());
            return false;
        }
        // load system font
        primary_font = TTF_OpenFont(PRIMARY_FONT_PATH, 14);
        secondary_font = TTF_OpenFont(SECONDARY_FONT_PATH, 14);
        if (!primary_font || !secondary_font) {
            logger->error("Failed to load font: {}", TTF_GetError());
            return false;
        }
        return true;
    }

    void cleanup() {
        TTF_CloseFont(primary_font);
        TTF_CloseFont(secondary_font);
        SDL_DestroyWindow(window);
    }
}
