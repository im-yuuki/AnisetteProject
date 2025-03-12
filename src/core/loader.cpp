//
// Created by Yuuki on 09/03/2025.
//
#include <core.h>
#include <logging.h>
#include <valarray>
#define logdebug logger->debug
#define loginfo logger->info
#define logwarn logger->warn
#define logerror logger->error

const auto logger = anisette::logging::get("loader");
constexpr uint32_t INIT_SUBSYSTEMS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS;

namespace anisette::core {
    VideoModule *video_module = nullptr;
    EventModule *event_module = nullptr;
    AudioModule *audio_module = nullptr;

    int run() {
        int status = 0;
        loginfo("Starting core");
        logdebug("Initializing SDL");
        if (SDL_InitSubSystem(INIT_SUBSYSTEMS) != 0) {
            logerror("Initialize SDL failed: {}", SDL_GetError());
            return 1;
        }
        logdebug("Initializing modules");
        video_module = new VideoModule();
        event_module = new EventModule();
        audio_module = new AudioModule();
        const auto module_list = std::initializer_list<Module*>{video_module, audio_module, event_module};
        for (auto &m : module_list) {
            if (m->init()) continue;
            status = 2;
            goto cleanup;
        }
        SDL_Delay(5000);
        cleanup:
        loginfo("Shutting down");
        for (auto &m : module_list) {
            m->cleanup();
            delete m;
        }
        logdebug("Shutting down SDL");
        SDL_QuitSubSystem(INIT_SUBSYSTEMS);
        SDL_Quit();
        return status;
    }
}