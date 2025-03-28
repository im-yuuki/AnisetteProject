//
// Created by Yuuki on 19/02/2025.
//
#pragma once
#include <cstdint>
#include <SDL_render.h>
#include <abs.h>

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


    enum FPS_MODE : int {
        // higher than 0 means custom FPS limit
        VSYNC = 0, // enable vsync and limit to display refresh rate
        UNLIMITED = -1, // no limit
        HALF_DISPLAY = -2, // limit to half of display refresh rate
        DISPLAY = -3, // limit to display refresh rate
        X2_DISPLAY = -4, // limit to double of display refresh rate
        X4_DISPLAY = -5, // limit to quadruple of display refresh rate
        X8_DISPLAY = -6, // limit to octuple of display refresh rate
    };

    /**
     * @brief Set the frame rate limit mode
     *
     * @param value FPS mode
     */
    extern void set_fps_mode(FPS_MODE value);

    /**
     * @brief Insert a new frame handler to the core stack
     *
     * The core will redirect the frame update to the handler on the top of the stack.
     *
     * @param handler The frame handler to be inserted
     */
    extern void insert_handler(abstract::FrameHandler *handler);

    /**
     * @brief Remove the top frame handler from the core stack
     *
     * The core will redirect the frame update to the new top handler of the stack.
     */
    extern void remove_handler();

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

    extern void _handle_event(const uint64_t &start_frame);
    extern void _handle_frame(const uint64_t &start_frame);

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