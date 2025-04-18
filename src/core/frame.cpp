//
// Created by Yuuki on 25/03/2025.
//
#include <stack>
#include "background.h"
#include "frametime_overlay.h"
#include "core.h"
#include "internal.h"
#include "common.h"
#include "discord.h"
#include "logging.h"

#define MAXIMUM_EVENT_POLL_PER_FRAME 16

const static auto logger = anisette::logging::get("frame");

namespace anisette::core {
    uint64_t last_frame_time = 0;

    static std::stack<abstract::Screen*> screen_stack;
    static bool new_screen_flag = false, back_screen_flag = false;
    static uint64_t now = 0, start_frame = 0, target_next_frame = 0, next_discord_poll = 0;

    // scene manager
    void open(abstract::Screen *handler) {
        screen_stack.push(handler);
        logger->debug("Open new screen");
        new_screen_flag = true;
    }

    void back() {
        if (back_screen_flag) {
            logger->warn("Another back request is made in this frame");
            return;
        }
        logger->debug("Back to previous screen");
        back_screen_flag = true;
    }

    void event_handler(const uint64_t &start_frame, const SDL_Event &event, abstract::Screen *screen) {
        switch (event.type) {
            case SDL_QUIT:
                request_stop();
            break;
            default: screen->on_event(start_frame, event);
        }
    }

    void main_loop() {
        SDL_Event event;
        abstract::Screen* current_handler = nullptr;

        while (!stop_requested) {
            start_frame = SDL_GetPerformanceCounter();
            // poll discord rpc
            if (start_frame > next_discord_poll) {
                utils::discord::poll();
                next_discord_poll = start_frame + system_freq / 2;
            }

            if (screen_stack.empty()) {
                logger->error("No screen in the stack");
                return;
            }
            current_handler = screen_stack.top();

            // trigger hook if the screen is changed
            if (new_screen_flag || back_screen_flag) {
                current_handler->on_focus(now);
                new_screen_flag = false;
                back_screen_flag = false;
            }

            // listen for events
            for (int i = 0; i < MAXIMUM_EVENT_POLL_PER_FRAME; i++) {
                if (!SDL_PollEvent(&event)) break;
                event_handler(start_frame, event, current_handler);
            }
            // clear screen
            SDL_SetRenderTarget(video::renderer, nullptr);
            SDL_SetRenderDrawColor(video::renderer, 0, 0, 0, 255);
            SDL_RenderClear(video::renderer);
            // draw background
            if (background_instance) background_instance->draw(start_frame);
            // pass control to the current screen
            if (current_handler) current_handler->update(start_frame);
            // draw frame time overlay
            if (frame_time_overlay) frame_time_overlay->draw(start_frame);
            // render
            SDL_RenderFlush(video::renderer);
            SDL_RenderPresent(video::renderer);
            SDL_RenderFlush(video::renderer);

            // check if requested to back to previous screen
            if (back_screen_flag) {
                delete current_handler;
                screen_stack.pop();
            }

            // delay until next frame, and calculate the frame time
            target_next_frame = start_frame + target_frame_time;
            now = SDL_GetPerformanceCounter();
            if (now < target_next_frame) {
                last_frame_time = target_next_frame - start_frame;
                SDL_Delay((target_next_frame - now) * 1000 / system_freq);
            } else last_frame_time = now - start_frame;
        }
    }
}