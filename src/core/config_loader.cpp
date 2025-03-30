//
// Created by Yuuki on 31/03/2025.
//
#include "config.h"
#include <logging.h>
#include <fstream>
#include <SDL_events.h>
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#define CONFIG_FILE_NAME "settings.json"

const auto logger = anisette::logging::get("loader");

namespace anisette::core::config
{
    const uint32_t SDL_EVENT_SAVE_CONFIG_FAILURE = SDL_RegisterEvents(1);
    static std::mutex mutex;

    bool load() {
        std::lock_guard lock(mutex);
        // load config file
        rapidjson::Document doc;
        if (doc.Parse("config.json").HasParseError()) {
            if (const auto err_code = doc.GetParseError(); err_code == rapidjson::kParseErrorDocumentEmpty) {
                logger->warn("Config file is empty, using default values");
            } else {
                logger->error("Failed to parse config file: error {} at line {}", err_code, doc.GetErrorOffset());
            }
            return false;
        }
        // check for required fields
        if (!doc.HasMember("render_width") || !doc.HasMember("render_height") || !doc.HasMember("fps")) {
            logger->error("Missing required fields in config file");
            return false;
        }

        for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
            if (it->value.IsNull()) continue;
            const auto key = it->name.GetString();
            switch (it->value.GetType()) {
                case rapidjson::kNumberType:
                    if (key == "render_width") {
                        render_width = it->value.GetUint();
                    } else if (key == "render_height") {
                        render_height = it->value.GetUint();
                    } else if (key == "fps") {
                        fps = it->value.GetInt();
                    } else if (key == "sound_volume") {
                        sound_volume = it->value.GetUint();
                    } else if (key == "music_volume") {
                        music_volume = it->value.GetUint();
                    }
                    break;
                case rapidjson::kTrueType:
                case rapidjson::kFalseType:
                    if (key == "enable_discord_rpc") {
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
        doc.AddMember("fps", fps, allocator);
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
        rapidjson::Writer writer(wrapper);
        doc.Accept(writer);
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