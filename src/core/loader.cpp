//
// Created by Yuuki on 09/03/2025.
//
#include <core.h>
#include <csignal>
#include <logging.h>
#include <valarray>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_version.h>

const auto logger = anisette::logging::get("loader");
constexpr SDL_InitFlags INIT_SUBSYSTEMS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS;


namespace anisette::core {
    VideoModule *video_module = nullptr;
    EventModule *event_module = nullptr;
    AudioModule *audio_module = nullptr;

    void handle_interrupt(int signal) {
        logger->info("Received interrupt signal {}", signal);
        if (video_module != nullptr) video_module->stop_thread();
        if (event_module != nullptr) event_module->stop_thread();
    }

    int init() {
        signal(SIGINT, handle_interrupt);
        signal(SIGTERM, handle_interrupt);
        logger->debug("Initializing SDL subsystems with flags {}", INIT_SUBSYSTEMS);
        if (!SDL_InitSubSystem(INIT_SUBSYSTEMS)) {
            logger->error("Initialize SDL failed: {}", SDL_GetError());
            return 1;
        }
        logger->debug("Initializing modules");
        video_module = new VideoModule();
        audio_module = new AudioModule();
        event_module = new EventModule();
        return 0;
    }

    void start_thread() {
        ThreadModule* modules[] = {video_module, event_module};
        logger->info("Starting core threads");
        for (const auto module : modules) module->start_thread();
        logger->info("Waiting for core threads to stop");
        for (const auto module : modules) module->wait_for_thread_stop();
    }

    void cleanup() {
        logger->info("Shutting down");
        delete video_module;
        delete audio_module;
        delete event_module;
        logger->debug("Shutting down SDL");
        SDL_QuitSubSystem(INIT_SUBSYSTEMS);
        SDL_Quit();
    }

    int run() {
        int status = 0;
        logger->info("Simple DirectMedia Layer (SDL) version: {}.{}.{}",
            SDL_MAJOR_VERSION,
            SDL_MINOR_VERSION,
            SDL_MICRO_VERSION
            );
        status = init();
        if (!status) start_thread();
        cleanup();
        return status;
    }
}