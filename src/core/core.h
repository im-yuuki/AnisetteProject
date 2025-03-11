//
// Created by Yuuki on 19/02/2025.
//
#pragma once
#include <SDL2/SDL.h>

namespace anisette::core {
    /**
     * @brief Start the game core
     *
     * This function initializes the game core and starts the main game loop.
     * It processes command-line arguments and sets up necessary resources.
     *
     * @param argc Number of command-line arguments
     * @param argv Array of command-line arguments
     * @return Exit code, 0 for success, otherwise errors
     */
    int run();

    class Module {
    public:
        virtual bool init() = 0;
        virtual void cleanup() = 0;
    };

    class VideoModule : public Module {
    public:
        bool init() override;
        void cleanup() override;

        [[nodiscard]]
        SDL_Window *get_window() const;
    private:
        SDL_Window *window = nullptr;
    };

    class AudioModule : public Module {
        bool init() override;
        void cleanup() override;
    };

    class EventModule : public Module {
        bool init() override;
        void cleanup() override;
    };
}