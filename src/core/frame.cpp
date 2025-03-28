//
// Created by Yuuki on 25/03/2025.
//
#include "core.h"
#include <logging.h>
#include <stack>
#include <cassert>

constexpr int MAXIMUM_EVENT_POLL_PER_FRAME = 10;

const static auto logger = anisette::logging::get("frame_handler");
static std::stack<anisette::core::abstract::FrameHandler*> frame_handlers;

namespace anisette::core {
    // scene manager
    void insert_handler(abstract::FrameHandler *handler) {
        assert(handler == nullptr);
        frame_handlers.push(handler);
        logger->debug("New frame handler inserted");
    }

    void remove_handler() {
        frame_handlers.pop();
        logger->debug("Removed the last frame handler");
    }

    // handle event
    void _handle_event(const uint64_t &start_frame) {
        static SDL_Event event;
        for (int i = 0; i < MAXIMUM_EVENT_POLL_PER_FRAME; i++) {
            if (!SDL_PollEvent(&event)) break;
            switch (event.type) {
                case SDL_QUIT:
                    request_stop();
                    break;
                default:
                    const auto handler = frame_handlers.top();
                    if (handler == nullptr) {
                        logger->error("There is no frame handler in the stack, application will stop");
                        return request_stop();
                    }
                    handler->handle_event(start_frame, event);
                    break;
            }
        }
    }

    // handle frame
    void _handle_frame(const uint64_t &start_frame) {
        const auto handler = frame_handlers.top();
        if (handler == nullptr) {
            logger->error("There is no frame handler in the stack, application will stop");
            return request_stop();
        }
        handler->handle_frame(start_frame);
    }
}