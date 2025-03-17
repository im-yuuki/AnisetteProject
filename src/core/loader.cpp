//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include <logging.h>
#include <discord.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_version.h>

const auto logger = anisette::logging::get("loader");
constexpr SDL_InitFlags INIT_SUBSYSTEMS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS;
static std::atomic_bool stop_requested = false;

static const Uint64 system_freq = SDL_GetPerformanceFrequency(); // constant
static Uint64 target_video_frame_time = system_freq / 60;
static Uint64 target_event_tick_time = system_freq / 100;
static Uint64 target_discord_update_time = system_freq / 2;

namespace anisette::core {
    void handle_interrupt(int signal) {
        logger->info("Received interrupt signal {}", signal);
        request_stop();
    }

    bool init() {
        logger->info("Simple DirectMedia Layer (SDL) version: {}.{}.{}",
            SDL_MAJOR_VERSION,
            SDL_MINOR_VERSION,
            SDL_MICRO_VERSION
            );
        logger->debug("Initializing SDL subsystems with flags {}", INIT_SUBSYSTEMS);
        if (!SDL_InitSubSystem(INIT_SUBSYSTEMS)) {
            logger->error("Initialize SDL failed: {}", SDL_GetError());
            return false;
        }
        // init video and audio subsystems
        if (!event::init()) return false;
        if (!video::init()) return false;
        if (!audio::init()) return false;
        // Discord RPC
        utils::discord::init();
        return true;
    }

    void cleanup() {
        logger->info("Shutting down");
        utils::discord::cleanup();
        event::cleanup();
        audio::cleanup();
        video::cleanup();
        logger->debug("Shutting down SDL");
        SDL_QuitSubSystem(INIT_SUBSYSTEMS);
        SDL_Quit();
    }

    void loop() {
        static Uint64 now;
        static Uint64 next_tick;
        static Uint64 next_video_frame = SDL_GetPerformanceCounter();
        static Uint64 next_event_tick = SDL_GetPerformanceCounter();
        static Uint64 next_discord_update = SDL_GetPerformanceCounter();

        logger->debug("Entering main loop");
        while (!stop_requested) {
            if (now = SDL_GetPerformanceCounter(); now >= next_event_tick) {
                event::process_tick(now);
                next_event_tick = now + target_event_tick_time;
            }
            if (now = SDL_GetPerformanceCounter(); now >= next_video_frame) {
                video::process_frame();
                next_video_frame = now + target_video_frame_time;
            }
            if (now >= next_discord_update) {
                utils::discord::update();
                next_discord_update = now + target_discord_update_time;
            }
            // sleep to save CPU cycles
            now = SDL_GetPerformanceCounter();
            next_tick = std::min(next_discord_update, next_event_tick);
            if (next_tick > now) SDL_Delay((next_tick - now) * 1000 / system_freq);
        }
        logger->debug("Exited main loop");
    }

    int run() {
        if (init()) {
            // pass control to main loop
            loop();
            // cleanup resources
            cleanup();
            return 0;
        }
        logger->error("Initialization failed, exiting");
        cleanup();
        return 1;
    }

    void set_video_fps(const int fps) {
        logger->info("Setting video fps to {}", fps);
        target_video_frame_time = system_freq / fps;
    }

    void set_event_tps(const int tps) {
        logger->info("Setting event tps to {}", tps);
        target_event_tick_time = system_freq / tps;
    }

    void request_stop() {
        logger->info("Application stop requested");
        stop_requested = true;
    }
}