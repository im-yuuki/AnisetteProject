//
// Created by Yuuki on 19/02/2025.
//
#pragma once
#include <cstdint>

namespace anisette::core {
    /**
     * @brief Start the game core
     *
     * This function initializes the game core and starts the main game loop.
     * It processes command-line arguments and sets up necessary resources.
     *
     * @return Exit code, 0 for success, otherwise errors
     */
    int run();
    void handle_interrupt(int signal);
    void set_video_fps(int fps);
    void set_event_tps(int tps);
    void request_stop();

    namespace event {
        bool init();
        void cleanup();
        void process_tick(const uint64_t &counter);
    };

    namespace video {
        bool init();
        void cleanup();
        void process_frame();
    }

    namespace audio {
        bool init();
        void cleanup();
    }
}
