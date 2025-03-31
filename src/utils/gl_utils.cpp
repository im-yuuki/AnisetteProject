//
// Created by Yuuki on 31/03/2025.
//
#include "gl_utils.h"
#include <fstream>

namespace anisette::utils::gl {

    GLShader::GLShader(const char* filename, const GLenum shaderType) : shaderType(shaderType) {
        std::ifstream file(filename);
        if (!file.is_open()) throw std::runtime_error("Open shader file failed: " + std::string(filename));

        const std::string source((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());
        const char *source_cstr = source.c_str();

        obj = glCreateShader(shaderType);
        glShaderSource(obj, 1, &source_cstr, nullptr);
        glCompileShader(obj);

        GLint compileStatus;
        glGetShaderiv(obj, GL_COMPILE_STATUS, &compileStatus);
        if (compileStatus != GL_TRUE) {
#ifdef NDEBUG
            throw std::runtime_error("Shader compilation failed");
#else
            GLint logLength;
            glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<char> log(logLength);
            glGetShaderInfoLog(obj, logLength, &logLength, log.data());
            throw std::runtime_error("Shader compilation failed: " + std::string(log.begin(), log.end()));
#endif
            glDeleteShader(obj);
        }
    }

     GLShader::~GLShader() {
        glDeleteShader(obj);
    }

    GLProgram::GLProgram(const GLShader& shaders...) {
        obj = glCreateProgram();
        for (const auto& shader : shaders) {
            shaders_list.push_back(shader.get());
            glAttachShader(obj, shader.get());
        }
        glLinkProgram(obj);

        GLint linkStatus;
        glGetProgramiv(obj, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
#ifdef NDEBUG
            throw std::runtime_error("GL program link failed");
#else
            GLint logLength;
            glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<char> log(logLength);
            glGetShaderInfoLog(obj, logLength, &logLength, log.data());
            throw std::runtime_error("GL program link failed: " + std::string(log.begin(), log.end()));
#endif
            glDeleteProgram(obj);
            for (const auto& shader : shaders) {
                glDetachShader(obj, shader.get());
            }
        }
    }

    GLProgram::~GLProgram() {
        glDeleteProgram(obj);
        for (const auto& shader : shaders_list) {
            glDetachShader(obj, shader);
        }
    }

}