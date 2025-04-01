//
// Created by Yuuki on 25/03/2025.
//
#include "core.h"
#include "_internal.h"
#include "utils/logging.h"
#include "utils/discord.h"
#include "components/frametime_overlay.h"
#include <stack>
#include <cassert>

constexpr int MAXIMUM_EVENT_POLL_PER_FRAME = 10;
const static auto logger = anisette::logging::get("frame");

namespace anisette::core {
    static std::stack<abstract::FrameHandler*> frame_handlers;
    static uint64_t now = 0, start_frame = 0, target_next_frame = 0, next_discord_poll = 0;

    // scene manager
    void insert_handler(abstract::FrameHandler *handler) {
        assert(handler != nullptr);
        frame_handlers.push(handler);
        logger->debug("New frame handler inserted");
    }

    void remove_handler() {
        frame_handlers.pop();
        logger->debug("Removed the last frame handler");
    }

    void _main_loop() {
        const auto system_freq = SDL_GetPerformanceFrequency();
        SDL_Event event;
        abstract::FrameHandler* current_handler = nullptr;

        while (!_stop_requested) {
            start_frame = SDL_GetPerformanceCounter();
            // poll discord rpc
            if (start_frame > next_discord_poll) {
                utils::discord::poll();
                next_discord_poll = start_frame + system_freq / 2;
            }

            if (frame_handlers.empty()) {
                logger->error("No frame handler in the stack");
                return;
            }
            current_handler = frame_handlers.top();

            // listen for events
            for (int i = 0; i < MAXIMUM_EVENT_POLL_PER_FRAME; i++) {
                if (!SDL_PollEvent(&event)) break;
                switch (event.type) {
                    case SDL_QUIT: request_stop();
                    break;
                    default: current_handler->on_event(start_frame, event);
                }
            }
            // update screen
            current_handler->update(start_frame);
            // draw frame time overlay
            if (frame_time_overlay) frame_time_overlay->draw();
            // render
            SDL_RenderPresent(video::renderer);

            // delay until next frame, and calculate the frame time
            target_next_frame = start_frame + _target_frame_time;
            if (now = SDL_GetPerformanceCounter(); now < target_next_frame) {
                last_frame_time = target_next_frame - start_frame;
                SDL_Delay((target_next_frame - now) * 1000 / system_freq);
            } else last_frame_time = now - start_frame;
        }
    }
}