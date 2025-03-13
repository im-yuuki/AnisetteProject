//
// Created by Yuuki on 09/03/2025.
//

#include <core.h>
#include <logging.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_properties.h>

const auto logger = anisette::logging::get("video");
constexpr SDL_WindowFlags WINDOW_INIT_FLAGS = SDL_WINDOW_FULLSCREEN;

namespace anisette::core {
    VideoModule::VideoModule() {
        logger->debug("Initialize window with flags {}", WINDOW_INIT_FLAGS);
        this->window = SDL_CreateWindow(
            "Anisette",
            1280, 720,
            WINDOW_INIT_FLAGS
            );
        if (window == nullptr) {
            logger->error("Initialize window failed: {}", SDL_GetError());
            goto failed;
        }
        configure_window();
        logger->debug("Creating renderer");
        renderer = SDL_CreateRenderer(window, nullptr);
        if (renderer == nullptr) {
            logger->error("Create renderer failed: {}", SDL_GetError());
            goto failed;
        }
        return;
        failed: return;
    }

    VideoModule::~VideoModule() {
        stop_thread();
        logger->debug("Destroying window");
        SDL_DestroyWindow(window);
    }

    void VideoModule::configure_window() const {
        const SDL_DisplayID display_id = SDL_GetDisplayForWindow(window);
        if (display_id == 0) {
            logger->warn("Failed to get display ID for window");
            return;
        }
        const SDL_DisplayMode *display_mode = SDL_GetDesktopDisplayMode(display_id);
        logger->debug("Using display {}: {}x{}@{}Hz", display_id, display_mode->w, display_mode->h, display_mode->refresh_rate);
        SDL_SetWindowFullscreenMode(window, display_mode);
        if (!SDL_SyncWindow(window)) {
            logger->warn("Failed to configure window: {}", SDL_GetError());
        }
    }

    void VideoModule::thread() {
        logger->debug("Render thread started");
        while (!stop_flag) {
            int r = 0, g = 100, b = 200;
            r = (r + 1) % 256;
            g = (g + 1) % 256;
            b = (b + 1) % 256;
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
            // std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        logger->debug("Render thread stopped");
    }

}