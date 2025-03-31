//
// Created by Yuuki on 22/03/2025.
//
#pragma once
#include "core/abs.h"
#include "utils/gl_utils.h"

#include <core/core.h>

namespace anisette::screens {
    class TestScreen final : public core::abstract::FrameHandler {
    public:
        TestScreen();
        void handle_event(const uint64_t &start_frame, const SDL_Event &event) override;
        ~TestScreen() override;
        void handle_frame(const uint64_t &start_frame) override;
    private:
        utils::gl::GLProgram* shaderProgram;
        GLuint VAO, VBO, EBO;
    };

    inline void load() {
        core::register_startup_frame_handler([&]() {
            return new TestScreen();
        });
    }
}