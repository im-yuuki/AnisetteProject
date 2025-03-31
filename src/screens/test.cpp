//
// Created by Yuuki on 31/03/2025.
//
#include "register.h"
#include "core/config.h"
#include <GL/glew.h>
#include <utils/gl_utils.h>

namespace anisette::screens {
    TestScreen::TestScreen() {
        const utils::gl::GLShader shader1("shaders/parallax.vert", GL_VERTEX_SHADER);
        const utils::gl::GLShader shader2("shaders/parallax.frag", GL_FRAGMENT_SHADER);
        shaderProgram = new utils::gl::GLProgram({shader1, shader2});
        glUseProgram(shaderProgram->get());

        // VBO và VAO
        float vertices[] = {
                // X     Y      Z     U    V
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
                1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
                1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // Top-right
                -1.0f, 1.0f,  0.0f, 0.0f, 1.0f // Top-left
        };
        GLuint indices[] = {0, 1, 2, 2, 3, 0};


        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void TestScreen::handle_frame(const uint64_t &start_frame) {
        // Lấy vị trí chuột
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        const float normMouseX = (mouseX / static_cast<float>(core::config::render_width)) * 2.0f - 1.0f;
        const float normMouseY = 1.0f - (mouseY / static_cast<float>(core::config::render_height)) * 2.0f;
        glUniform2f(glGetUniformLocation(shaderProgram->get(), "uMouse"), normMouseX, normMouseY);
        glUniform1f(glGetUniformLocation(shaderProgram->get(), "blurStrength"), 0.1f);

        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    void TestScreen::handle_event(const uint64_t &start_frame, const SDL_Event &event) {
    }

    TestScreen::~TestScreen() {
        delete shaderProgram;
    }
}
