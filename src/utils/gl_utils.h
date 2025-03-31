//
// Created by Yuuki on 31/03/2025.
//
#pragma once
#include "logging.h"
#include <GL/glew.h>

namespace anisette::utils::gl
{
    class GLObjectWrapper {
    public:
        [[nodiscard]] GLuint get() const { return obj; }
        virtual ~GLObjectWrapper() = default;
    protected:
        GLuint obj = 0;
    };

    class GLShader final : public GLObjectWrapper {
    public:
        const GLenum shaderType;
        GLShader(const char* filename, GLenum shaderType);
        ~GLShader() override;
    };

    class GLProgram final : public GLObjectWrapper {
    public:
        explicit GLProgram(const GLShader& shaders...);
        ~GLProgram() override;
    protected:
        std::vector<GLenum> shaders_list;
    };
}
