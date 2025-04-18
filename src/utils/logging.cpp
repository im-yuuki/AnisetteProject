//
// Created by Yuuki on 19/02/2025.
//
#include "logging.h"
#include <string>
using namespace spdlog;

#ifdef NDEBUG
#include <spdlog/sinks/basic_file_sink.h>
static std::string generate_filename() {
    using namespace std::chrono;
    const auto now = system_clock::to_time_t(system_clock::now());
    return "log/anisette-run-" + std::to_string(now) + ".log";
}
const auto output_sink = std::make_shared<sinks::basic_file_sink_mt>(generate_filename(), true);
constexpr level::level_enum default_level = level::info;
#else
#include <spdlog/sinks/stdout_color_sinks.h>
const auto output_sink = std::make_shared<sinks::stdout_color_sink_mt>();
constexpr level::level_enum default_level = level::debug;
#endif
namespace anisette::logging
{
    void init() {
        output_sink->set_level(default_level);
    }

    std::shared_ptr<logger> get(const std::string &name) {
        const auto _return = std::make_shared<logger>(logger(name, output_sink));
        _return->set_level(default_level);
        _return->set_pattern("[%d-%m-%Y %H:%M:%S] %n [%^%l%$] %v");
        return _return;
    }
}