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

        obj_id = glCreateShader(shaderType);
        glShaderSource(obj_id, 1, &source_cstr, nullptr);
        glCompileShader(obj_id);

        GLint compileStatus;
        glGetShaderiv(obj_id, GL_COMPILE_STATUS, &compileStatus);
        if (compileStatus != GL_TRUE) {
#ifdef NDEBUG
            throw std::runtime_error("Shader compilation failed");
#else
            GLint logLength;
            glGetShaderiv(obj_id, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<char> log(logLength);
            glGetShaderInfoLog(obj_id, logLength, &logLength, log.data());
            throw std::runtime_error("Shader compilation failed: " + std::string(log.begin(), log.end()));
#endif
            glDeleteShader(obj_id);
        }
    }

     GLShader::~GLShader() {
        glDeleteShader(obj_id);
    }

    GLProgram::GLProgram(const std::initializer_list<GLShader> shaders) {
        obj_id = glCreateProgram();
        for (const auto& shader : shaders) {
            shaders_list.push_back(shader.get());
            glAttachShader(obj_id, shader.get());
        }
        glLinkProgram(obj_id);

        GLint linkStatus;
        glGetProgramiv(obj_id, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
#ifdef NDEBUG
            throw std::runtime_error("GL program link failed");
#else
            GLint logLength;
            glGetShaderiv(obj_id, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<char> log(logLength);
            glGetShaderInfoLog(obj_id, logLength, &logLength, log.data());
            throw std::runtime_error("GL program link failed: " + std::string(log.begin(), log.end()));
#endif
            glDeleteProgram(obj_id);
            for (const auto& shader : shaders) {
                glDetachShader(obj_id, shader.get());
            }
        }
    }

    GLProgram::~GLProgram() {
        glDeleteProgram(obj_id);
        for (const auto& shader : shaders_list) {
            glDetachShader(obj_id, shader);
        }
    }

}