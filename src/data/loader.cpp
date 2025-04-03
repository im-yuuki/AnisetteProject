//
// Created by Yuuki on 02/04/2025.
//
#include "data.h"
#include "logging.h"
#include <filesystem>
#include <thread>

#define BEATMAPS_ROOT_DIR "beatmaps"

const auto logger = anisette::logging::get("data");

namespace anisette::data
{
    void BeatmapLoader::scan(const SortStrategy sort_strategy, bool ascending) {
        std::thread t([this, sort_strategy, ascending]() {
            index.clear();
            beatmaps.clear();
            load_finished = false;
            logger->debug("Scanning beatmaps");
            for (const auto& entry : std::filesystem::directory_iterator(BEATMAPS_ROOT_DIR)) {
                if (!entry.is_directory()) continue;
                for (const auto& file : std::filesystem::directory_iterator(entry.path())) {
                    if (file.path().extension() != ".json") continue;
                    Beatmap beatmap;
                    if (!beatmap.load(file.path().filename().string(), entry.path().string())) continue;
                    beatmaps.push_back(beatmap);
                }
            }
            logger->debug("Scanning beatmaps finished");
            if (sort_strategy != NONE) {
                // sort
                std::ranges::sort(beatmaps, [sort_strategy, ascending](const Beatmap &a, const Beatmap &b) {
                    bool res = false;
                    switch (sort_strategy) {
                        case BY_ID:
                            res = a.id > b.id;
                        case BY_TITLE:
                            res = a.title > b.title;
                        case BY_ARTIST:
                            res = a.artist > b.artist;
                        case BY_DIFFICULTY:
                            res = a.difficulty > b.difficulty;
                        break;
                        default: break;
                    }
                    return res ^ !ascending;
                });
            }
            // end
            load_finished = true;
            logger->debug("Load beatmaps finished");
        });
        t.detach();
    }

    bool BeatmapLoader::is_scan_finished() {
        if (load_finished) logger->debug("Beatmap scan finished");
        return load_finished;
    }

    Beatmap* BeatmapLoader::get_beatmap(const int id) {
        if (!load_finished) return nullptr;
        if (const auto it = index.find(id); it != index.end()) {
            return &beatmaps[it->second];
        }
        return nullptr;
    }
}