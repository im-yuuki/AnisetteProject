//
// Created by Yuuki on 19/02/2025.
//

#include "logging.h"
#include <iostream>

const auto &LOGGING_OSTREAM = std::cout;

namespace game::logging {
    void debug(const std::string &message) {
        return default_logger->debug(message);
    }

    void info(const std::string &message) {
        return default_logger->info(message);
    }

    void warning(const std::string &message) {
        return default_logger->warning(message);
    }

    void error(const std::string &message) {
        return default_logger->error(message);
    }

    void set_default_level(const LogLevel level) {
        default_logger->set_level(level);
    }

    Logger* Logger::get(const std::string &name) {
        const auto find = pool.find(name);
        if (find == pool.end()) {
            const auto logger = new Logger(name);
            pool[name] = logger;
            if (name == "default") return logger;
            return logger->set_level(default_logger->level);
        }
        return find->second;
    }

    Logger *Logger::set_level(LogLevel level) {
        return this;
    }

    void Logger::debug(const std::string &message) const {
        std::cout << name << " [DEBUG] " << message << std::endl;
    }
    void Logger::info(const std::string &message) const {
        std::cout << name << " [INFO] " << message << std::endl;
    }
    void Logger::warning(const std::string &message) const {
        std::cerr << name << " [WARNING] " << message << std::endl;
    }
    void Logger::error(const std::string &message) const {
        std::cerr << name << " [ERROR] " << message << std::endl;
    }

}
