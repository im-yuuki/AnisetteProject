//
// Created by Yuuki on 19/02/2025.
//
#include <logging.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
using namespace anisette::logging;

static const char* generate_filename() {
    using namespace std::chrono;
    const auto now = system_clock::to_time_t(system_clock::now());
    return ("log/anisette-run-" + std::to_string(now) + ".log").c_str();
}

const char *file_name = generate_filename();
static bool initialized = false;
static Level default_level = INFO;
static std::mutex console_mutex, file_mutex;

const char* level_string(const Level &level) {
    switch (level) {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        default: return "INFO";
    }
}

const char* colored_level_string(const Level &level) {
    switch (level) {
        case DEBUG: return "\033[1;34mDEBUG\033[0m";
        case INFO: return "\033[1;32mINFO\033[0m";
        case WARNING: return "\033[1;33mWARNING\033[0m";
        case ERROR: return "\033[1;31mERROR\033[0m";
        default: return "\033[1;32mINFO\033[0m";
    }
}

void write_console(const std::string &message) {
    std::lock_guard lock(console_mutex);
    std::cout << message << std::endl;
}

void write_file(const std::string &message) {
    std::lock_guard lock(file_mutex);
    std::ofstream file(file_name, std::ios::app, std::ios::out);
    file << message << std::endl;
}

namespace anisette::logging {
    bool init(const Level level) {
        if (initialized) return false;
        if (level != DEFAULT) default_level = level;
        if (const std::ofstream file(file_name, std::ios::app, std::ios::out); !file.is_open()) {
            std::cerr << "[FATAL] Failed to open target log file " << file_name << std::endl;
            return false;
        }
        initialized = true;
        return true;
    }

    std::shared_ptr<Logger> Logger::create(const std::string &name, const Level &level) {
        return std::make_shared<Logger>(name, level);
    }

    Logger::Logger(const std::string &name, const Level &level) : name(name), level(level) {}

    template <class... T>
        void Logger::debug(const std::format_string<T...> &fmt, T &&...args) {
        log(DEBUG, fmt, std::forward<T>(args)...);
    }

    template <class... T>
    void Logger::info(const std::format_string<T...> &fmt, T &&...args) {
        log(INFO, fmt, std::forward<T>(args)...);
    }

    template <class... T>
    void Logger::warn(const std::format_string<T...> &fmt, T &&...args) {
        log(WARNING, fmt, std::forward<T>(args)...);
    }

    template <class... T>
    void Logger::error(const std::format_string<T...> &fmt, T &&...args) {
        log(ERROR, fmt, std::forward<T>(args)...);
    }

    template <class... T>
    void Logger::log(Level level, const std::format_string<T...> &fmt, T &&...args) {
        if (!initialized) return;
        if (level == DEFAULT) level = this->level;
        if (level < default_level) return;
        const std::string message = std::format(fmt, std::forward<T>(args)...);
        std::string console_out = format("{} [{}] ", name, colored_level_string(level));
        std::string file_out = format("{} [{}] ", name, level_string(level));
        write_console(console_out.append(message));
        write_file(file_out.append(message));
    }

}
