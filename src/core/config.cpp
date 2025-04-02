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

#define CONFIG_FILE_NAME "settings.json"

const auto logger = anisette::logging::get("loader");

namespace anisette::core::config
{
     bool is_fallback = true;
    // default values
     int render_width = 1600;
     int render_height = 900;
     FPS_VALUE fps = X2_DISPLAY;
     DISPLAY_MODE display_mode = EXCLUSIVE;
     uint8_t sound_volume = 128;
     uint8_t music_volume = 128;
     bool show_frametime_overlay = true;
     bool enable_discord_rpc = true;

    bool load() {
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
                        render_width = it->value.GetInt();
                        if (render_width < 640) render_width = 640;
                    } else if (strcmp(key, "render_height") == 0) {
                        render_height = it->value.GetInt();
                        if (render_height < 480) render_height = 480;
                    } else if (strcmp(key, "fps") == 0) {
                        fps = it->value.GetInt();
                        if (fps < X8_DISPLAY) fps = X8_DISPLAY;
                    } else if (strcmp(key, "sound_volume") == 0) {
                        sound_volume = it->value.GetUint();
                    } else if (strcmp(key, "music_volume") == 0) {
                        music_volume = it->value.GetUint();
                    } else if (strcmp(key, "display_mode") == 0) {
                        switch (it->value.GetUint()) {
                            case EXCLUSIVE:
                                display_mode = EXCLUSIVE;
                                break;
                            case BORDERLESS:
                                display_mode = BORDERLESS;
                                break;
                            case WINDOWED:
                            default:
                                display_mode = WINDOWED;
                                break;
                        }
                    }
                    break;
                case rapidjson::kTrueType:
                case rapidjson::kFalseType:
                    if (strcmp(key, "enable_discord_rpc") == 0) {
                        enable_discord_rpc = it->value.GetBool();
                    } else if (strcmp(key, "show_frametime_overlay") == 0) {
                        show_frametime_overlay = it->value.GetBool();
                    }
                default: break;
            }
        }
        is_fallback = false;
        return true;
    }

    void validate() {
        // minimum resolution is 800x600
        if (render_width < 800 || render_height < 600) {
            logger->warn("Invalid resolution config");
            render_width = 1280;
            render_height = 720;
        }
        if (fps < -10) {
            logger->warn("Invalid FPS config");
            fps = DISPLAY;
        }
    }


    bool save(const bool quiet) {
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
        doc.AddMember("show_frametime_overlay", show_frametime_overlay, allocator);
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
        if (!quiet) logger->info("Config file saved successfully");
        return true;
    }
}