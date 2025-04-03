//
// Created by Yuuki on 09/03/2025.
//
#include "core.h"
#include "internal.h"
#include "config.h"
#include "static.h"
#include "utils/logging.h"
#include "utils/discord.h"

#include <csignal>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_version.h>
#include <SDL_mixer.h>

const auto logger = anisette::logging::get("core");

constexpr uint32_t INIT_SUBSYSTEMS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
constexpr uint32_t SDL_IMAGE_INIT_FLAGS = IMG_INIT_PNG | IMG_INIT_JPG;
constexpr uint32_t SDL_MIXER_INIT_FLAGS = MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_WAVPACK;

namespace anisette::core
{
    std::atomic_bool stop_requested = false;
    uint64_t target_frame_time = 0;
    components::FrameTimeOverlay *frame_time_overlay = nullptr;
    components::Background *background_instance = nullptr;
    
    static std::function<abstract::Screen*(SDL_Renderer*)> register_function;

    void handle_interrupt(int signal) {
        logger->debug("Received interrupt signal {}", signal);
        request_stop();
    }

    void register_first_screen(const std::function<abstract::Screen*(SDL_Renderer* renderer)> &reg_fn) {
        register_function = reg_fn;
    }

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
        // register event filter
        SDL_SetEventFilter([](void *userdata, SDL_Event *event) {
            switch (event->type) {
                case SDL_QUIT:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    return 1;

                default:
                    return 0;
            }
        }, nullptr);
        // init video and audio handlers
        if (!(audio::init() && video::init())) return false;
        // post-init task
        logger->debug("Load background instance");
        background_instance = new components::Background(video::renderer);
        reload_config();
        open(register_function(video::renderer));
        return true;
    }

    /**
     * @brief Cleanup all resources before exiting
     */
    void cleanup() {
        logger->info("Shutting down");
        // stop discord rpc
        utils::discord::shutdown();
        // free background instance
        delete background_instance;
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

    void load_background(const std::string &path, const uint64_t &now) {
        if (background_instance) {
            background_instance->load(path, now);
        } else {
            logger->error("Background instance is not initialized");
        }
    }

    void toggle_background_parallax(const bool enable) {
        if (background_instance) {
            background_instance->enable_parallax = enable;
        } else {
            logger->error("Background instance is not initialized");
        }
    }

    void reload_config() {
        assert(video::renderer);
        // const auto display_mode = video::get_display_mode();
        // reload fps value
        if (config::fps == config::VSYNC) {
            logger->debug("FPS is set to VSync mode");
            SDL_RenderSetVSync(video::renderer, true);
            target_frame_time = system_freq / 2000;
        } else {
            SDL_RenderSetVSync(video::renderer, false);
            if (config::fps > 0) {
                logger->debug("FPS: {}", config::fps);
                target_frame_time = system_freq / config::fps;
            } else if (config::fps == config::UNLIMITED) {
                logger->warn("FPS is set to unlimited mode, can lead to high resource usage");
                target_frame_time = 0;
            } else if (config::fps == config::DISPLAY) {
                logger->debug("FPS is set to match display refresh rate");
                target_frame_time = system_freq / video::display_mode.refresh_rate;
            } else if (config::fps == config::X2_DISPLAY) {
                logger->debug("FPS is set to 2x of display refresh rate");
                target_frame_time = system_freq / video::display_mode.refresh_rate / 2;
            } else if (config::fps == config::X4_DISPLAY) {
                logger->debug("FPS is set to 4x of display refresh rate");
                target_frame_time = system_freq / video::display_mode.refresh_rate / 4;
            } else if (config::fps == config::X8_DISPLAY) {
                logger->debug("FPS is set to 8x of display refresh rate");
                target_frame_time = system_freq / video::display_mode.refresh_rate / 8;
            } else if (config::fps == config::HALF_DISPLAY) {
                logger->debug("FPS is set to half of display refresh rate");
                target_frame_time = system_freq / video::display_mode.refresh_rate * 2;
            }
        }
        // frame time overlay
        if (config::show_frametime_overlay && !frame_time_overlay) {
            logger->debug("Frame time overlay enabled");
            frame_time_overlay = new components::FrameTimeOverlay(video::renderer);
        } else {
            logger->debug("Frame time overlay disabled");
            delete frame_time_overlay;
            frame_time_overlay = nullptr;
        }
        // discord rpc
        if (config::enable_discord_rpc) utils::discord::start();
        else utils::discord::shutdown();
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
        main_loop();
        logger->debug("Exited main loop");

        quit:
        cleanup();
        return err;
    }

    void request_stop() {
        logger->debug("Core stop requested");
        stop_requested = true;
    }
}