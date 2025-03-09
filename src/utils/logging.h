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
     * @param level Default log level   
     */
    void init(const uint8_t level = LEVEL_INFO);

    /**
     * @brief Get the logger instance by name
     * @param name Logger name
     * @return Logger instance
     */
    std::shared_ptr<spdlog::logger> get(const std::string &name);

    /**
     * @brief Get the logger instance by name and set the custom log level
     * @param name Logger name
     * @param level Log level
     * @return Logger instance
     */ 
    std::shared_ptr<spdlog::logger> get(const std::string &name, const uint8_t level);
}