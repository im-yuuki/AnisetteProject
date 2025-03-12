//
// Created by Yuuki on 09/03/2025.
//

#include <core.h>
#include <logging.h>
#include <SDL3_image/SDL_image.h>
#define logdebug logger->debug
#define loginfo logger->info
#define logwarn logger->warn
#define logerror logger->error

const auto logger = anisette::logging::get("video");
constexpr SDL_WindowFlags WINDOW_INIT_FLAGS = SDL_WINDOW_OPENGL;

namespace anisette::core {
    bool VideoModule::init() {
        // logdebug("Initialize SDL_Image");
        // if (IMG_Init(IMAGE_INIT_FLAGS) != IMAGE_INIT_FLAGS) {
        //     logerror("Initialize image loader failed: {}", IMG_GetError());
        //     return false;
        // }
        logdebug("Initialize window");
        window = SDL_CreateWindow(
            "Anisette",
            1280, 720,
            WINDOW_INIT_FLAGS
            );
        if (window == nullptr) {
            logerror("Initialize window failed: {}", SDL_GetError());
            return false;
        }
        return true;
    }

    void VideoModule::cleanup() {
        logdebug("Destroying window");
        if (window != nullptr) SDL_DestroyWindow(window);
        // logdebug("Shutting down SDL_Image");
        // IMG_Quit();
    }

    SDL_Window *VideoModule::get_window() const {
        if (window == nullptr) logwarn("Window is not initialized");
        return window;
    }
}