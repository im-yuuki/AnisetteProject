//
// Created by Yuuki on 09/03/2025.
//
#include <core.h>
#include <logging.h>
#include <SDL3/SDL_events.h>

#ifndef EVENT_PER_TICK
#define EVENT_PER_TICK 10
#endif

const auto logger = anisette::logging::get("event");
static SDL_Event ev;

namespace anisette::core::event {
    void process_tick() {
        static int i;
        for (i = 0; i < EVENT_PER_TICK; i++) {
            if (!SDL_PollEvent(&ev)) break;
            switch (ev.type) {
                case SDL_EVENT_QUIT:
                    logger->info("Received quit event");
                request_stop();
                break;
                default: break;
            }
        }

    }
}