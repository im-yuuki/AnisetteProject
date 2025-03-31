//
// Created by Yuuki on 22/03/2025.
//
#pragma once
#include "core/abs.h"

#include <core/core.h>

namespace anisette::screens {
    class TestScreen final : public core::abstract::FrameHandler {
    public:
        TestScreen();
        void on_event(const uint64_t &start_frame, const SDL_Event &event) override;
        ~TestScreen() override;
        void update(const uint64_t &start_frame) override;
    private:
    };

    inline void load() {
        core::register_first_frame_handler([&]() {
            return new TestScreen();
        });
    }
}