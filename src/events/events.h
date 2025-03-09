//
// Created by Yuuki on 09/03/2025.
//

#pragma once

namespace anisette::events {
    class EventLoop {
    public:
        /**
         * @brief Start the event loop
         */
        void start();

        /**
         * @brief Stop the event loop
         */
        void stop();
    private:
        bool running = false;
    };
}