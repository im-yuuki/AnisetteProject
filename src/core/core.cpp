//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include <logging.h>
#include <discord.h>
#include <stack>
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
        // start discord rpc
        utils::discord::start();
        // init video and audio handlers
        return video::splash() && audio::init() && video::init();
    }

    void cleanup() {
        logger->info("Shutting down");
        // stop discord rpc
        utils::discord::shutdown();
        // quit handlers
        audio::cleanup();
        video::cleanup();
        // quit SDL
        logger->debug("Shutting down SDL");
        SDL_QuitSubSystem(INIT_SUBSYSTEMS);
        SDL_Quit();
    }

    void loop() {
        logger->debug("Entering main loop");
        static uint64_t now = 0, start_frame = 0, target_next_frame = 0, next_discord_poll = 0;

        while (!stop_requested) {
            start_frame = SDL_GetPerformanceCounter();
            // poll discord rpc
            if (start_frame > next_discord_poll) {
                utils::discord::poll();
                next_discord_poll = start_frame + system_freq / 2;
            }
            // handle frame
            SDL_SetRenderDrawColor(video::renderer, 0, 0, 0, 255);
            SDL_RenderClear(video::renderer);
            SDL_RenderPresent(video::renderer);
            // handle events
            // delay until next frame, and calculate the frame time
            target_next_frame = start_frame + target_frametime;
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

    void handle_interrupt(int signal) {
        logger->debug("Received interrupt signal {}", signal);
        request_stop();
    }
}