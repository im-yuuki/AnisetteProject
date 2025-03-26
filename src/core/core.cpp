//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include <logging.h>
#include <discord.h>
#include <csignal>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_version.h>

const auto logger = anisette::logging::get("core");
constexpr uint32_t INIT_SUBSYSTEMS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;

static std::atomic_bool stop_requested = false;
static uint64_t target_frame_time = 0;

namespace anisette::core
{
    const uint64_t system_freq = SDL_GetPerformanceFrequency();

    bool init() {
        signal(SIGINT, handle_interrupt);
        signal(SIGTERM, handle_interrupt);
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

    void set_fps_mode(const FPS_MODE mode) {
        assert(video::renderer != nullptr);
        if (mode == VSYNC) {
            logger->info("FPS is now in VSync mode");
            SDL_RenderSetVSync(video::renderer, true);
            target_frame_time = system_freq / video::display_mode.refresh_rate;
            return;
        }
        SDL_RenderSetVSync(video::renderer, false);
        switch (mode) {
            case UNLIMITED:
                logger->warn("FPS is no longer being limited. This may cause high resources usage.");
            target_frame_time = 0;
            break;
            case DISPLAY:
                logger->info("FPS: {}", video::display_mode.refresh_rate);
            target_frame_time = system_freq / video::display_mode.refresh_rate;
            break;
            case HALF_DISPLAY:
                logger->info("FPS: {}", video::display_mode.refresh_rate / 2);
            target_frame_time = system_freq / (video::display_mode.refresh_rate * 2);
            break;
            default: break;
        }
    }

    // entrypoint
    int run() {
        int err = 0;
        if (!init()) {
            logger->error("Initialization failed, exiting");
            err = 1;
            goto quit;
        }

        logger->debug("Entering main loop");
        uint64_t now = 0, start_frame = 0, target_next_frame = 0, next_discord_poll = 0;

        while (!stop_requested) {
            start_frame = SDL_GetPerformanceCounter();
            // poll discord rpc
            if (start_frame > next_discord_poll) {
                utils::discord::poll();
                next_discord_poll = start_frame + system_freq / 2;
            }

            handle_event(start_frame);
            handle_frame(start_frame);

            // delay until next frame, and calculate the frame time
            target_next_frame = start_frame + target_frame_time;
            if (now = SDL_GetPerformanceCounter(); now < target_next_frame) {
                last_frame_time = target_next_frame - start_frame;
                SDL_Delay((target_next_frame - now) * 1000 / system_freq);
            } else last_frame_time = now - start_frame;
        }

        logger->debug("Exited main loop");

        quit:
        cleanup();
        return err;
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