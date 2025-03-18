//
// Created by Yuuki on 19/02/2025.
//
#pragma once
#include <cstdint>

namespace anisette::core
{
    /**
     * @brief The frequency of the system's performance counter.
     *
     * This variable holds the frequency of the system's performance counter,
     * which is used for timing and measuring performance.
     */
    extern const uint64_t system_freq;

    extern unsigned target_fps;
    extern void commit_update();

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

    namespace video
    {
        extern bool splash();
        extern bool init();
        extern void cleanup();
        extern void process_frame(const uint64_t &counter);
    }

    namespace audio
    {
        extern bool init();
        extern void cleanup();
        extern void set_sound_volume(int volume);
        extern void set_music_volume(int volume);
    }
}
