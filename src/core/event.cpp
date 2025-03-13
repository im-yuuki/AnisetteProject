//
// Created by Yuuki on 09/03/2025.
//
#include <core.h>
#include <logging.h>

const auto logger = anisette::logging::get("event");

namespace anisette::core {
    EventModule::EventModule() {
    }

    EventModule::~EventModule() {
        stop_thread();
    }

    void EventModule::thread() {
        logger->debug("Event loop started");
        while (!stop_flag) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        logger->debug("Event loop stopped");
    }

}