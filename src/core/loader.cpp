//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include <logging.h>
#include <discord.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_version.h>

const auto logger = anisette::logging::get("loader");
constexpr uint32_t INIT_SUBSYSTEMS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
static std::atomic_bool stop_requested = false;

namespace anisette::core
{
    const Uint64 system_freq = SDL_GetPerformanceFrequency();
    unsigned target_fps = 60;

    static uint64_t target_frametime = system_freq / target_fps;
    uint64_t frame_time;

    void apply_settings() {
        if (target_fps == 0) {
            target_frametime = 0;
            logger->warn("FPS is basically unlimited, may lead to high resources usage");
        } else {
            target_frametime = system_freq / target_fps;
            logger->info("FPS limit changed to {}", target_fps);
        }
    }

    bool init() {
        logger->info("Simple DirectMedia Layer (SDL) version: {}.{}.{}", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
        if (SDL_InitSubSystem(INIT_SUBSYSTEMS)) {
            logger->error("Initialize SDL failed: {}", SDL_GetError());
            return false;
        }
        // init video and audio subsystems
        return video::splash() && event::init() && audio::init() && video::init();
    }

    void cleanup() {
        logger->info("Shutting down");
        event::cleanup();
        audio::cleanup();
        video::cleanup();
        logger->debug("Shutting down SDL");
        SDL_QuitSubSystem(INIT_SUBSYSTEMS);
        SDL_Quit();
    }

    void loop() {
        logger->debug("Entering main loop");
        static uint64_t now, start_frame, target_next_frame;

        while (!stop_requested) {
            start_frame = SDL_GetPerformanceCounter();
            event::process_tick(start_frame);
            video::process_frame(start_frame);
            target_next_frame = start_frame + target_frametime;
            // delay until next frame, and calculate the frame time
            if (now = SDL_GetPerformanceCounter(); now < target_next_frame) {
                frame_time = target_next_frame - start_frame;
                SDL_Delay((target_next_frame - now) * 1000 / system_freq);
            } else frame_time = now - start_frame;
        }

        logger->debug("Exited main loop");
    }

    int run() {
        if (init()) {
            loop();
            cleanup();
            return 0;
        }
        logger->error("Initialization failed, exiting");
        cleanup();
        return 1;
    }

    void request_stop() {
        logger->info("Core stop requested");
        stop_requested = true;
    }
}