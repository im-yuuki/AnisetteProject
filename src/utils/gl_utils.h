//
// Created by Yuuki on 31/03/2025.
//
#pragma once
#include "logging.h"
#include <initializer_list>
#include <GL/glew.h>

namespace anisette::utils::gl
{
    class GLObjectWrapper {
    public:
        [[nodiscard]] GLuint get() const { return obj_id; }
        virtual ~GLObjectWrapper() = default;
    protected:
        GLuint obj_id = 0;
    };

    class GLShader final : public GLObjectWrapper {
    public:
        const GLenum shaderType;
        GLShader(const char* filename, GLenum shaderType);
        ~GLShader() override;
    };

    class GLProgram final : public GLObjectWrapper {
    public:
        explicit GLProgram(std::initializer_list<GLShader> shaders);
        ~GLProgram() override;
    protected:
        std::vector<GLenum> shaders_list;
    };
}
