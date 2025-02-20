//
// Created by Yuuki on 19/02/2025.
//

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#define LOGGING_HISTORY_SIZE 1000
#define LOGGING_DEFAULT_LEVEL INFO

namespace game::logging {
    // Log levels
    enum LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3
    };

    // Record of a log message
    struct LogRecord {
        const LogLevel level;
        std::string message;
    };

    class Logger final {
    public:
        static Logger* get(const std::string &name);
        void debug(const std::string &message) const;
        void info(const std::string &message) const;
        void warning(const std::string &message) const;
        void error(const std::string &message) const;
        Logger* set_level(LogLevel level);
    private:
        explicit Logger(std::string name) : name(std::move(name)) {};
        inline static std::unordered_map<std::string, Logger*> pool;
        std::vector<int> subscribers;
        const LogLevel level = LOGGING_DEFAULT_LEVEL;
        const std::string name;
    };

    extern void debug(const std::string &message);
    extern void info(const std::string &message);
    extern void warning(const std::string &message);
    extern void error(const std::string &message);

    extern void set_default_level(LogLevel level);

    // Default logger, to handle global logging functions
    inline const auto default_logger = Logger::get("default");
}