//
// Created by Yuuki on 19/02/2025.
//
#include <logging.h>
#include <string>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
using namespace anisette::logging;

static bool initialized = false;
static spdlog::level::level_enum default_level = spdlog::level::debug;

static std::string generate_filename() {
    using namespace std::chrono;
    const auto now = system_clock::to_time_t(system_clock::now());
    return "log/anisette-run-" + std::to_string(now) + ".log";
}

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

const static auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
const static auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(generate_filename(), true);

namespace anisette::logging {
    void init(const Level level) {
        using namespace spdlog;
        if (initialized) return;
        default_level = convert(level);
        set_default_logger(get("default"));
        initialized = true;
    }

    std::shared_ptr<spdlog::logger> get(const std::string &name, const Level level) {
        const auto logger = std::make_shared<spdlog::logger>(spdlog::logger(name, { console_sink, file_sink }));
        logger->set_level(convert(level));
        logger->set_pattern("[%d-%m-%Y %H:%M:%S] %n [%^%l%$] %v");
        return logger;
    }
}