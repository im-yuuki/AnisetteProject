//
// Created by Yuuki on 19/02/2025.
//

#pragma once
#include <spdlog/spdlog.h>

/**
 * Anisette logging module
 *
 * @author Yuuki
 * @date 19/02/2025
 * @version 1.0.0
 */
namespace anisette::logging
{
    /**
     * @brief Initialize the logging system
     *
     * This function initializes the logging system with the default log level.
     * It sets up the console or file sinks for logging output.
     *
     * @note This function must be called before any other logging function.
     */
    void init();

    /**
     * @brief Get the logger instance by name
     *
     * @param name Logger name
     * @return Logger instance
     */
    [[nodiscard]]
    std::shared_ptr<spdlog::logger> get(const std::string &name);

    /**
     * @brief Register a callback function for logging messages
     *
     * This function registers a callback function that will be called for each warning log message.
     * Used to display warning notification in the user interface.
     *
     * @param function Callback function
     */
    void register_callback(const std::function<void(const spdlog::details::log_msg &)> &function);
}