//
// Created by Yuuki on 03/04/2025.
//
#include "data.h"
#include "utils/logging.h"
#include <thread>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

// these fields are used for data validation
#define SCHEMA_VERSION 1
#define SCHEMA_URL "https://raw.githubusercontent.com/im-yuuki/AnisetteProject/refs/heads/sdl2/scripts/beatmap.schema.json"

const auto logger = anisette::logging::get("beatmap");

namespace anisette::data
{
    Beatmap::Beatmap(const std::string &file, const std::string &dir) : path(path) {
        load_result = NOT_FINISHED;
        std::thread t ([this]() {
            std::ifstream file(this->path);
            if (!file.is_open()) {
                logger->error("Failed to open beatmap file: {}", this->path);
                load_result = FAILED;
                return;
            }
            rapidjson::IStreamWrapper wrapper(file);
            rapidjson::Document doc;
            if (doc.ParseStream(wrapper).HasParseError()) {
                logger->error("Failed to parse beatmap file: {}", this->path);
                load_result = FAILED;
                return;
            }
            bool flow = doc.HasMember("$schema")
                && doc["$schema"].IsString()
                && strcmp(doc["$schema"].GetString(), SCHEMA_URL) == 0
                && doc.HasMember("version")
                && doc["version"].IsInt()
                && doc["version"].GetInt() == SCHEMA_VERSION;
            if (!flow) {
                logger->error("Beatmap file is not valid: {}", this->path);
                load_result = FAILED;
                return;
            }
            try {
                // TODO: better validating these fields
                id = doc["id"].GetUint();
                title = doc["title"].GetString();
                artist = doc["artist"].GetString();
                thumbnail_path = this->path + '/' + doc["thumbnail"].GetString();
                music_path = this->path + '/' + doc["music"].GetString();
                preview_point = doc["preview_point"].GetUint();
                difficulty = doc["difficulty"].GetUint();
                hp_drain = doc["hp_drain"].GetUint();

                auto note_section = doc["notes"].GetObject();
                auto channel_0 = note_section["channel_0"].GetArray();
                auto channel_1 = note_section["channel_1"].GetArray();
                auto channel_2 = note_section["channel_2"].GetArray();
                auto channel_3 = note_section["channel_3"].GetArray();
                auto channel_4 = note_section["channel_4"].GetArray();
                auto channel_5 = note_section["channel_5"].GetArray();

                for (auto &note : channel_0) {
                    auto start = note[0].GetUint();
                    auto end = note[1].GetUint();
                    notes[0].push_back({start, end});
                }
                for (auto &note : channel_1) {
                    auto start = note[0].GetUint();
                    auto end = note[1].GetUint();
                    notes[1].push_back({start, end});
                }
                for (auto &note : channel_2) {
                    auto start = note[0].GetUint();
                    auto end = note[1].GetUint();
                    notes[2].push_back({start, end});
                }
                for (auto &note : channel_3) {
                    auto start = note[0].GetUint();
                    auto end = note[1].GetUint();
                    notes[3].push_back({start, end});
                }
                for (auto &note : channel_4) {
                    auto start = note[0].GetUint();
                    auto end = note[1].GetUint();
                    notes[4].push_back({start, end});
                }
                for (auto &note : channel_5) {
                    auto start = note[0].GetUint();
                    auto end = note[1].GetUint();
                    notes[5].push_back({start, end});
                }
            } catch (const std::exception &e) {
                logger->error("Failed to load beatmap file: {}", e.what());
                load_result = FAILED;
                return;
            }
            load_result = SUCCESS;
        });
        t.detach();
    }

    AsyncLoadResult Beatmap::is_load_finished() const {
        return load_result;
    }

}