//
// Created by Yuuki on 09/03/2025.
//
#include <discord.h>

#include "core.h"
#include <logging.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>

const auto logger = anisette::logging::get("event");

namespace anisette::core::event
{
    bool init() {
        SDL_SetEventFilter([](void *userdata, SDL_Event *event) {
            return 1;
        }, nullptr);
        // start discord rpc
        utils::discord::start();
        return true;
    }

    void cleanup() {
        utils::discord::shutdown();
    }

    void handle_interrupt(int signal) {
        logger->debug("Received interrupt signal {}", signal);
        request_stop();
    }

    void warning_callback(const spdlog::details::log_msg &msg) {
        SDL_Event ev;
        SDL_PushEvent(&ev);
    }

    void process_tick(const uint64_t &counter) {
        static SDL_Event ev;
        if (!SDL_PollEvent(&ev)) return;
        switch (ev.type) {
            case SDL_QUIT:
                request_stop();
                break;
            default: break;
        }
    }
}