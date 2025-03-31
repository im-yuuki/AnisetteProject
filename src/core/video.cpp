//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include "config.h"
#include "utils/logging.h"
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

const auto logger = anisette::logging::get("video");
namespace anisette::core::video
{

    bool init() {
        // target opengl 3.3
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        // Initialize
        logger->debug("Initializing main window");
        uint32_t flags = SDL_WINDOW_OPENGL;
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
        // init OpenGL context
        glewExperimental = GL_TRUE;
        glewInit();
        gl_context = SDL_GL_CreateContext(window);
        if (!gl_context) {
            logger->error("Create OpenGL context failed: {}", SDL_GetError());
            return false;
        }
        return refresh_display_info();
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
