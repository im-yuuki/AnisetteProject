//
// Created by Yuuki on 19/02/2025.
//

#include "config.h"
#include <logging>
#include <fstream>
#include <sstream>

#define DEFAULT_RENDER_CONFIG_FILE_PATH "render.cfg"

const auto logger = game::logging::Logger::get(__FILE__);

using namespace game::core;

RenderConfigLoader::RenderConfigLoader() {
    if (!read(DEFAULT_RENDER_CONFIG_FILE_PATH)) {
        logger->warning("Failed to load render configuration, using fallback mode");
    } else {
        fallback_mode = false;
    }
}

bool PreferencesLoader::read(const char *file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) return false;
    // Read line by line
    unsigned int line_number = 0;
    std::string line;
    while (std::getline(file, line)) {
        line_number++;
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string key, value;
        ss >> key >> value;
        if (key.empty() || value.empty() || !_import(std::make_pair(key, value))) {
            logger->warning("Read " + std::string(file_path) + ": Syntax error in line " + std::to_string(line_number));
        }
    }
    logger->debug("Read " + std::string(file_path) + ": " + std::to_string(line_number) + " lines loaded");
    file.close();
    return true;
}

bool PreferencesLoader::write(const char *file_path) {
    std::ofstream file(file_path);
    if (!file.is_open()) return false;
    file.close();
    return true;
}

bool PreferencesLoader::save() {
    return write(DEFAULT_RENDER_CONFIG_FILE_PATH);
}

bool RenderConfigLoader::is_using_fallback_mode() const {
    return fallback_mode;
}
