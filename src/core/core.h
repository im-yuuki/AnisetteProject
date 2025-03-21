//
// Created by Yuuki on 19/02/2025.
//
#pragma once
#include <cstdint>
#include <SDL_render.h>
#include <string>

/**
 * @brief Anisette game core, the soul of this project
 *
 * @author Yuuki
 * @version 1.0.0
 */
namespace anisette::core
{
    /**
     * @brief The frequency of the system's performance counter.
     *
     * This is simply value of `SDL_GetPerformanceFrequency()`.
     * I keep it here for easy access and avoid calling the function multiple times.
     */
    extern const uint64_t system_freq;

    /**
     * @brief Frame time of the latest handle
     */
    inline uint64_t last_frame_time = 0;


    enum FPS_MODE {
        UNLIMITED,
        VSYNC,
        DISPLAY,
        HALF_DISPLAY,
        CUSTOM
    };

    /**
     * @brief Set the frame rate limit mode
     *
     * @param mode FPS mode
     */
    extern void set_fps_mode(FPS_MODE mode);

    extern bool load_scene(const std::string &scene_name);
    extern void back_scene();

    /**
     * @brief Start the game core
     *
     * This function initializes the game core and starts the main game loop.
     * It processes command-line arguments and sets up necessary resources.
     *
     * @return Exit code, 0 for success, otherwise errors
     */
    extern int run();

    /**
     * @brief Request the game core to stop
     *
     * This function set the flag to stop the main game loop.
     * It will then trigger the cleanup process and exit the game.
     */
    extern void request_stop();

    /**
     * @brief Handle interrupt signal
     *
     * This function is called when the game receives an interrupt signal.
     * It will request the game core to stop and trigger the cleanup process.
     *
     * @param signal Signal number
     */
    extern void handle_interrupt(int signal);

    /**
     * @brief Handler for rendering and displaying task
     */
    namespace video
    {
        /**
         * @brief Global renderer
         */
        inline SDL_Renderer *renderer = nullptr;
        inline SDL_DisplayMode display_mode;

        /**
         * @brief Initialize dependencies, load splash screen and fetch the display info
         * 
         * @return true if all are success, false otherwise
         */
        extern bool splash();

        /**
         * 
         * @return true if success, false otherwise
         */
        extern bool init();
        extern void cleanup();
        extern bool refresh_display_info();
    }

    /**
     * @brief Handler for audio output
     */
    namespace audio
    {
        extern bool init();
        extern void cleanup();
        extern void set_sound_volume(int volume);
        extern void set_music_volume(int volume);

        extern bool play_music(const char* path);
        extern void pause_music();
        extern void resume_music();
        extern void stop_music();
    }
}