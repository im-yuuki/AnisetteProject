//
// Created by Yuuki on 09/03/2025.
//
#include <core.h>
#include <csignal>
#include <logging.h>
#include <valarray>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_version.h>

const auto logger = anisette::logging::get("loader");
constexpr SDL_InitFlags INIT_SUBSYSTEMS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS;
static std::atomic_bool stop_requested = false;

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
        if (video::init()) return false;
        if (audio::init()) return false;
        return true;
    }

    void cleanup() {
        logger->info("Shutting down");
        audio::cleanup();
        video::cleanup();
        logger->debug("Shutting down SDL");
        SDL_QuitSubSystem(INIT_SUBSYSTEMS);
        SDL_Quit();
    }

    void loop() {
        while (!stop_requested) {
            event::process_tick();
            video::process_frame();
        }
    }

    int run() {
        if (!init()) {
            loop();
            cleanup();
            return 0;
        }
        logger->error("Initialization failed, exiting");
        cleanup();
        return 1;
    }

    void set_video_fps(const int fps) {

    }

    void set_event_tps(const int tps) {

    }

    void request_stop() {
        stop_requested = true;
    }
}