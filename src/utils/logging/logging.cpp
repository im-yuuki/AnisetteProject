//
// Created by Yuuki on 19/02/2025.
//
#include "logging.h"
#include <string>
using namespace anisette::logging;

#ifdef NDEBUG
#include <spdlog/sinks/basic_file_sink.h>
static std::string generate_filename() {
    using namespace std::chrono;
    const auto now = system_clock::to_time_t(system_clock::now());
    return "log/anisette-run-" + std::to_string(now) + ".log";
}
const static auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(generate_filename(), true);
static constexpr spdlog::level::level_enum default_level = spdlog::level::debug;
#else
#include <spdlog/sinks/stdout_color_sinks.h>
const static auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
static constexpr spdlog::level::level_enum default_level = spdlog::level::debug;
#endif

static spdlog::level::level_enum convert(const Level level) {
    switch (level) {
        case DEBUG: return spdlog::level::debug;
        case INFO: return spdlog::level::info;
        case WARNING: return spdlog::level::warn;
        case ERROR: return spdlog::level::err;
        case DEFAULT: break;
    }
    return default_level;
}

namespace anisette::logging {
    void init() {
        set_default_logger(get("default"));
    }

    std::shared_ptr<spdlog::logger> get(const std::string &name, const Level level) {
        const auto logger = std::make_shared<spdlog::logger>(spdlog::logger(name, sink));
        logger->set_level(convert(level));
        logger->set_pattern("[%d-%m-%Y %H:%M:%S] %n [%^%l%$] %v");
        return logger;
    }
}