//
// Created by Yuuki on 09/03/2025.
//
#include <core.h>
#include <logging.h>
#include <valarray>

const auto logger = anisette::logging::get("loader");
constexpr uint32_t INIT_SUBSYSTEMS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS;

namespace anisette::core {
    VideoModule *video_module = nullptr;
    EventModule *event_module = nullptr;
    AudioModule *audio_module = nullptr;

    int run() {
        int status = 0;
        logger->info("Starting core");
        logger->debug("Initializing SDL");
        if (!SDL_InitSubSystem(INIT_SUBSYSTEMS)) {
            logger->error("Initialize SDL failed: {}", SDL_GetError());
            return 1;
        }
        logger->debug("Initializing modules");
        video_module = new VideoModule();
        event_module = new EventModule();
        audio_module = new AudioModule();
        SDL_Delay(5000);
        cleanup:
        logger->info("Shutting down");
        for (auto &m : std::valarray<Module *>({video_module, event_module, audio_module})) {
            m->cleanup();
            delete m;
        }
        logger->debug("Shutting down SDL");
        SDL_QuitSubSystem(INIT_SUBSYSTEMS);
        SDL_Quit();
        return status;
    }
}