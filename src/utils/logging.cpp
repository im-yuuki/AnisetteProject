//
// Created by Yuuki on 19/02/2025.
//
#include "logging.h"
#include <string>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
using namespace anisette::logging;

static spdlog::level::level_enum default_level = spdlog::level::info;
static bool initialized = false;

static std::string generate_filename() {
    using namespace std::chrono;
    const auto now = system_clock::to_time_t(system_clock::now());
    return "log/anisette-run-" + std::to_string(now) + ".log";
}

static spdlog::level::level_enum convert(const uint8_t level) {
    switch (level) {
        case LEVEL_DEBUG:
            return spdlog::level::debug;
        case LEVEL_INFO:
            return spdlog::level::info;
        case LEVEL_WARN:
            return spdlog::level::warn;
        case LEVEL_ERROR:
            return spdlog::level::err;
        default:
            return default_level;
    }
}

const static auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
const static auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(generate_filename(), true);

namespace anisette::logging {
    void init(const uint8_t level) {
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        default_level = convert(level);
        spdlog::set_level(default_level);
        spdlog::set_pattern("[%d-%m-%Y %H:%M:%S] %n [%^%l%$] %v");
        spdlog::set_default_logger(get("default", level));
        initialized = true;
    }

    std::shared_ptr<spdlog::logger> get(const std::string &name, const uint8_t level) {
        const auto logger = std::make_shared<spdlog::logger>(spdlog::logger(name, { console_sink, file_sink }));
        if (level != 255) logger->set_level(convert(level));
        return logger;
    }
}