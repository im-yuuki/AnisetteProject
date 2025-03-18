//
// Created by Yuuki on 19/02/2025.
//
#pragma once
#include <cstdint>

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
     * @brief Target FPS (frames per second) of the game
     *
     * This variable holds the target FPS of the game, which is used to limit
     * the frame rate of the game. Set to 0 to disable frame rate limiting.
     *
     * @note Call `apply_settings()` after changing this value to apply.
     */
    extern unsigned target_fps;

    /**
     * @brief Apply new settings to the game core
     */
    extern void apply_settings();

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
     * @brief Submodule for event-related features
     */
    namespace event
    {
        extern bool init();
        extern void cleanup();

        /**
         * @brief Handle interrupt signal
         *
         * This function is called when the game receives an interrupt signal.
         * It will request the game core to stop and trigger the cleanup process.
         *
         * @param signal Signal number
         */
        extern void handle_interrupt(int signal);
        extern void process_tick(const uint64_t &counter);
    };

    /**
     * @brief Submodule for rendering and displaying task
     */
    namespace video
    {
        extern bool splash();
        extern bool init();
        extern void cleanup();
        extern bool refresh_display_info();
        extern void process_frame(const uint64_t &counter);
    }

    /**
     * @brief Submodule for audio output
     */
    namespace audio
    {
        extern bool init();
        extern void cleanup();
        extern void set_sound_volume(int volume);
        extern void set_music_volume(int volume);
    }
}
