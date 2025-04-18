//
// Created by Yuuki on 03/04/2025.
//
#include "data.h"
#include "logging.h"
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

// these fields are used for data validation
#define SCHEMA_VERSION 1
#define SCHEMA_URL "https://raw.githubusercontent.com/im-yuuki/AnisetteProject/refs/heads/sdl2/scripts/beatmap.schema.json"

const auto logger = anisette::logging::get("beatmap");

namespace anisette::data
{
    bool Beatmap::load(const std::string &filename, const std::string &dir) {
        path = dir + '/' + filename;
        for (auto &note : notes) {
            note.clear();
        }
        logger->debug("Loading beatmap {}", filename);
        std::ifstream file(this->path);
        if (!file.is_open()) {
            logger->error("Failed to open beatmap file: {}", filename);
            return false;
        }
        rapidjson::IStreamWrapper wrapper(file);
        rapidjson::Document doc;
        if (doc.ParseStream(wrapper).HasParseError()) {
            logger->error("Failed to parse beatmap file: {}", filename);
            file.close();
            return false;
        }
        bool flow = doc.HasMember("$schema")
            && doc["$schema"].IsString()
            && strcmp(doc["$schema"].GetString(), SCHEMA_URL) == 0
            && doc.HasMember("version")
            && doc["version"].IsInt()
            && doc["version"].GetInt() == SCHEMA_VERSION;
        if (!flow) {
            logger->error("Beatmap file is not valid: {}", filename);
            return false;
        }
        try {
            // TODO: better validating these fields
            id = doc["id"].GetUint();
            title = doc["title"].GetString();
            artist = doc["artist"].GetString();
            thumbnail_path = dir + '/' + doc["thumbnail"].GetString();
            music_path = dir + '/' + doc["music"].GetString();
            preview_point = doc["preview_point"].GetUint();
            difficulty = doc["difficulty"].GetUint();
            hp_drain = doc["hp_drain"].GetUint();

            auto note_section = std::move(doc["notes"]);
            single_note_count = note_section["single_note_count"].GetInt();
            hold_note_count = note_section["hold_note_count"].GetInt();

            auto channel_0 = note_section["channel_0"].GetArray();
            for (auto &note : channel_0) {
                auto start = note[0].GetInt();
                auto end = note[1].GetInt();
                notes[0].push_back({start, end});
            }

            auto channel_1 = note_section["channel_1"].GetArray();
            for (auto &note : channel_1) {
                auto start = note[0].GetInt();
                auto end = note[1].GetInt();
                notes[1].push_back({start, end});
            }

            auto channel_2 = note_section["channel_2"].GetArray();
            for (auto &note : channel_2) {
                auto start = note[0].GetInt();
                auto end = note[1].GetInt();
                notes[2].push_back({start, end});
            }

            auto channel_3 = note_section["channel_3"].GetArray();
            for (auto &note : channel_3) {
                auto start = note[0].GetInt();
                auto end = note[1].GetInt();
                notes[3].push_back({start, end});
            }

            auto channel_4 = note_section["channel_4"].GetArray();
            for (auto &note : channel_4) {
                auto start = note[0].GetInt();
                auto end = note[1].GetInt();
                notes[4].push_back({start, end});
            }

            auto channel_5 = note_section["channel_5"].GetArray();
            for (auto &note : channel_5) {
                auto start = note[0].GetInt();
                auto end = note[1].GetInt();
                notes[5].push_back({start, end});
            }
        } catch (const std::exception &e) {
            logger->error("Failed to load beatmap file {}: {}", filename, e.what());
            file.close();
            return false;
        }
        file.close();
        return true;
    }
}