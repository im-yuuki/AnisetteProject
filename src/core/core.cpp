//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include "config.h"
#include "utils/logging.h"
#include "utils/discord.h"

#include <csignal>
#include <SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_version.h>

const auto logger = anisette::logging::get("core");

constexpr uint32_t INIT_SUBSYSTEMS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
constexpr uint32_t SDL_IMAGE_INIT_FLAGS = IMG_INIT_PNG | IMG_INIT_JPG;
constexpr uint32_t SDL_MIXER_INIT_FLAGS = MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_WAVPACK;

namespace anisette::core
{
    static std::atomic_bool _stop_requested = false;
    static uint64_t _target_frame_time = 0;
    const uint64_t system_freq = SDL_GetPerformanceFrequency();

    /**
     * @brief Handle interrupt signal
     *
     * This function is called when the game receives an interrupt signal.
     * It will request the game core to stop and trigger the cleanup process.
     *
     * @param signal Signal number
     */
    void handle_interrupt(int signal) {
        logger->debug("Received interrupt signal {}", signal);
        request_stop();
    }

    /**
     * @brief Initialize eveything of the game core
     *
     * @return true if all are success, false otherwise
     */
    bool init() {
        // register signal handlers
        signal(SIGINT, handle_interrupt);
        signal(SIGTERM, handle_interrupt);
        // load config
        config::load();

        // initialize SDL and its modules
        logger->info("Simple DirectMedia Layer (SDL) version: {}.{}.{}", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
        if (SDL_InitSubSystem(INIT_SUBSYSTEMS)) {
            logger->error("Initialize SDL failed: {}", SDL_GetError());
            return false;
        }
        logger->info("SDL_image version: {}.{}.{}", SDL_IMAGE_MAJOR_VERSION, SDL_IMAGE_MINOR_VERSION, SDL_IMAGE_PATCHLEVEL);
        if (IMG_Init(SDL_IMAGE_INIT_FLAGS) != (SDL_IMAGE_INIT_FLAGS)) {
            logger->error("SDL_image init failed: {}", SDL_GetError());
            return false;
        }
        logger->info("SDL_ttf version: {}.{}.{}", SDL_TTF_MAJOR_VERSION, SDL_TTF_MINOR_VERSION, SDL_IMAGE_PATCHLEVEL);
        if (TTF_Init()) {
            logger->error("SDL_ttf init failed: {}", SDL_GetError());
            return false;
        }
        logger->info("SDL_mixer version: {}.{}.{}", SDL_MIXER_MAJOR_VERSION, SDL_MIXER_MINOR_VERSION, SDL_MIXER_PATCHLEVEL);
        if (Mix_Init(SDL_MIXER_INIT_FLAGS) != (SDL_MIXER_INIT_FLAGS)) {
            logger->error("Failed to initialize audio mixer");
            return false;
        }
        // start discord rpc
        if (config::enable_discord_rpc) utils::discord::start();
        // init video and audio handlers
        if (audio::init() && video::init()) {
            set_fps(config::fps);
            return true;
        }
        return false;
    }

    /**
     * @brief Cleanup all resources before exiting
     */
    void cleanup() {
        logger->info("Shutting down");
        // stop discord rpc
        utils::discord::shutdown();
        // quit handlers
        audio::cleanup();
        video::cleanup();
        // quit SDL
        logger->debug("Shutting down SDL");
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_QuitSubSystem(INIT_SUBSYSTEMS);
        SDL_Quit();
        // save config
        config::save();
    }

    void set_fps(const int value) {
        assert(video::renderer != nullptr);
        if (value == config::VSYNC) {
            logger->debug("FPS is now in VSync mode");
            SDL_RenderSetVSync(video::renderer, true);
            _target_frame_time = system_freq / video::display_mode.refresh_rate;
            return;
        }
        SDL_RenderSetVSync(video::renderer, false);
        if (value > 0) {
            logger->debug("FPS: {}", value);
            _target_frame_time = system_freq / value;
            return;
        }
        switch (value) {
            case config::UNLIMITED:
                logger->warn("FPS is now in unlimited mode");
                _target_frame_time = 0;
                break;
            case config::DISPLAY:
                logger->debug("FPS is match to display refresh rate");
                _target_frame_time = system_freq / video::display_mode.refresh_rate;
                break;
            case config::X2_DISPLAY:
                logger->debug("FPS is 2x of display refresh rate");
                _target_frame_time = system_freq / video::display_mode.refresh_rate / 2;
                break;
            case config::X4_DISPLAY:
                logger->debug("FPS is 2x of display refresh rate");
                _target_frame_time = system_freq / video::display_mode.refresh_rate / 4;
                break;
            case config::X8_DISPLAY:
                logger->debug("FPS is 2x of display refresh rate");
                _target_frame_time = system_freq / video::display_mode.refresh_rate / 8;
                break;
            case config::HALF_DISPLAY:
                logger->debug("FPS is half of display refresh rate");
                _target_frame_time = system_freq / video::display_mode.refresh_rate * 2;
                break;
            default: break;
        }

    }

    // entrypoint
    int run() {
        int err = 0;
        uint64_t now = 0, start_frame = 0, target_next_frame = 0, next_discord_poll = 0;

        if (!init()) {
            logger->error("Initialization failed, exiting");
            err = 1;
            goto quit;
        }

        logger->debug("Entering main loop");

        while (!_stop_requested) {
            start_frame = SDL_GetPerformanceCounter();
            // poll discord rpc
            if (start_frame > next_discord_poll) {
                utils::discord::poll();
                next_discord_poll = start_frame + system_freq / 2;
            }

            _handle_event(start_frame);
            _handle_frame(start_frame);

            // delay until next frame, and calculate the frame time
            target_next_frame = start_frame + _target_frame_time;
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
        logger->debug("Core stop requested");
        _stop_requested = true;
    }
}