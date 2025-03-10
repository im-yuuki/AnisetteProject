//
// Created by Yuuki on 19/02/2025.
//
#pragma once
#include <SDL2/SDL.h>
#include <memory>

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
    int run(int argc, char *argv[]);

    namespace video {
        /**
         * @brief Initialize the video subsystem and create the main window
         *
         * This function initializes the SDL video subsystem and creates the main
         * application window. It should be called before any rendering operations.
         *
         * @return True if the window was successfully created, false otherwise
         */
        bool init_window();

        /**
         * @brief Get the main application window
         *
         * This function returns a shared pointer to the main SDL window created
         * by the `init_window` function. It can be used to perform rendering
         * operations or to query window properties.
         *
         * @return Shared pointer to the main SDL window
         */
        [[nodiscard]]
        std::shared_ptr<SDL_Window> get_window();
    }

    namespace audio {
        bool init();
    }

    namespace event {

    }
}