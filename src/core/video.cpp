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
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        if (!renderer) {
            logger->error("Initialize renderer failed: {}", SDL_GetError());
            return false;
        }
        return fetch_display_info();
    }

    bool fetch_display_info() {
        if (SDL_GetDesktopDisplayMode(SDL_GetWindowDisplayIndex(window), &display_mode)) {
            logger->error("Fetch display mode failed: {}", SDL_GetError());
            return false;
        }
        logger->info("Display info: {}x{}@{}Hz", display_mode.w, display_mode.h, display_mode.refresh_rate);
        return true;
    }

    void cleanup() {
        SDL_DestroyWindow(window);
    }

    SDL_Rect get_overlay_render_position(const RenderPositionX position_x, const RenderPositionY position_y,
                                         const int width, const int height, const int margin_x, const int margin_y) {
        auto ans = SDL_Rect{};
        ans.w = width;
        ans.h = height;
        switch (position_x) {
            case LEFT:
                ans.x = margin_x;
                break;
            case CENTER:
                ans.x = (config::render_width - width) / 2;
                break;
            case RIGHT:
                ans.x = config::render_width - width - margin_x;
                break;
            default:
                ans.x = 0;
        }
        switch (position_y) {
            case TOP:
                ans.y = margin_y;
                break;
            case MIDDLE:
                ans.y = (config::render_height - height) / 2;
                break;
            case BOTTOM:
                ans.y = config::render_height - height - margin_y;
                break;
            default:
                ans.y = 0;
        }
        return ans;
    }
}
