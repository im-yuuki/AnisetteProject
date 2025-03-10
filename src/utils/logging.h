//
// Created by Yuuki on 19/02/2025.
//

#pragma once
#include <spdlog/spdlog.h>

namespace anisette::logging {
    constexpr uint8_t LEVEL_DEBUG = 0;
    constexpr uint8_t LEVEL_INFO = 1;
    constexpr uint8_t LEVEL_WARN = 2;
    constexpr uint8_t LEVEL_ERROR = 3;

    /**
     * @brief Initialize the logging system
     *
     * This function initializes the logging system with the specified default log level.
     * It sets up the console and file sinks for logging output.
     *
     * @param level Default log level
     */
    void init(uint8_t level = LEVEL_INFO);

    /**
     * @brief Get the logger instance by name
     *
     * This function returns a shared pointer to the logger instance with the specified name.
     * If a custom log level is provided, it will be used for the logger; otherwise, the default level will be used.
     *
     * @param name Logger name
     * @param level Custom log level, if not provided, the default level will be used
     * @return Logger instance
     */
    [[nodiscard]]
    std::shared_ptr<spdlog::logger> get(const std::string &name, uint8_t level = 255);
}