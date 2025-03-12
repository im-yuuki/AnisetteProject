//
// Created by Yuuki on 09/03/2025.
//

#include <core.h>
#include <logging.h>
#include <SDL3_image/SDL_image.h>

const auto logger = anisette::logging::get("video");
constexpr SDL_WindowFlags WINDOW_INIT_FLAGS = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;

namespace anisette::core {
    VideoModule::VideoModule() {
        logger->debug("Initialize window");
        this->window = SDL_CreateWindow(
            "Anisette",
            1280, 720,
            WINDOW_INIT_FLAGS
            );
        if (window == nullptr) {
            logger->error("Initialize window failed: {}", SDL_GetError());
            goto failed;
        }
        logger->debug("Creating renderer");
        renderer = SDL_CreateRenderer(window, nullptr);
        if (renderer == nullptr) {
            logger->error("Create renderer failed: {}", SDL_GetError());
            goto failed;
        }
        return;
        failed: return;
    }

    void VideoModule::cleanup() {
        logger->debug("Destroying window");
        SDL_DestroyWindow(window);
    }

    SDL_Window *VideoModule::get_window() const {
        if (window == nullptr) logger->warn("Window is not initialized");
        return window;
    }
}