//
// Created by Yuuki on 31/03/2025.
//
#include "config.h"
#include "utils/logging.h"
#include <fstream>
#include <SDL_events.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#define CONFIG_FILE_NAME "anisette_settings.json"

const auto logger = anisette::logging::get("loader");

namespace anisette::core::config
{
    const uint32_t SDL_EVENT_SAVE_CONFIG_FAILURE = SDL_RegisterEvents(1);
    static std::mutex mutex;

    bool load() {
        std::lock_guard lock(mutex);
        // load config file
        logger->debug("Loading config file");
        std::ifstream config_file(CONFIG_FILE_NAME);
        if (!config_file.is_open()) {
            logger->warn("Config file not found, fallback to default values");
            return false;
        }
        rapidjson::IStreamWrapper wrapper(config_file);
        // parse config file
        rapidjson::Document doc;
        if (doc.ParseStream(wrapper).HasParseError()) {
            logger->warn("Failed to parse config file at line {}", doc.GetErrorOffset());
            return false;
        }
        for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
            if (it->value.IsNull()) continue;
            const auto key = it->name.GetString();
            switch (it->value.GetType()) {
                case rapidjson::kNumberType:
                    if (strcmp(key, "render_width") == 0) {
                        render_width = it->value.GetUint();
                    } else if (strcmp(key, "render_height") == 0) {
                        render_height = it->value.GetUint();
                    } else if (strcmp(key, "fps_value") == 0) {
                        fps = it->value.GetInt();
                    } else if (strcmp(key, "sound_volume") == 0) {
                        sound_volume = it->value.GetUint();
                    } else if (strcmp(key, "music_volume") == 0) {
                        music_volume = it->value.GetUint();
                    }
                    break;
                case rapidjson::kTrueType:
                case rapidjson::kFalseType:
                    if (strcmp(key, "enable_discord_rpc") == 0) {
                        enable_discord_rpc = it->value.GetBool();
                    }
                default: break;
            }
        }
        is_fallback = false;
        return true;
    }

    bool save(const bool quiet) {
        std::lock_guard lock(mutex);
        if (!quiet) logger->debug("Saving config file");
        rapidjson::Document doc;
        doc.SetObject();
        rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
        doc.AddMember("render_width", render_width, allocator);
        doc.AddMember("render_height", render_height, allocator);
        doc.AddMember("fps_value", fps, allocator);
        doc.AddMember("sound_volume", sound_volume, allocator);
        doc.AddMember("music_volume", music_volume, allocator);
        doc.AddMember("enable_discord_rpc", enable_discord_rpc, allocator);
        // save to file
        std::ofstream ofs(CONFIG_FILE_NAME);
        if (!ofs.is_open()) {
            if (!quiet) logger->error("Failed to open config file for writing: {}", CONFIG_FILE_NAME);
            return false;
        }
        rapidjson::OStreamWrapper wrapper(ofs);
        rapidjson::PrettyWriter writer(wrapper);
        doc.Accept(writer);
        ofs.close();
        if (!quiet) logger->debug("Config file saved successfully");
        return true;
    }

    void save_async() {
        logger->debug("Saving config file asynchronously");
        std::thread t([&]() {
            if (save(true)) return;
            SDL_PushEvent(new SDL_Event{.type = SDL_EVENT_SAVE_CONFIG_FAILURE});
        });
    }
}