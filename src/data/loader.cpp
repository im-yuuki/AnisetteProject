//
// Created by Yuuki on 02/04/2025.
//
#include "data.h"
#include <filesystem>
#include <thread>

#define BEATMAPS_ROOT_DIR "beatmaps"

namespace anisette::data
{
    void BeatmapLoader::scan(const SortStrategy sort_strategy, bool ascending) {
        std::thread t([this, sort_strategy, ascending]() {
            index.clear();
            beatmaps.clear();
            load_finished = false;
            std::vector<Beatmap> beatmap_list;
            for (const auto& entry : std::filesystem::directory_iterator(BEATMAPS_ROOT_DIR)) {
                if (!entry.is_directory()) continue;
                for (const auto& file : std::filesystem::directory_iterator(entry.path())) {
                    if (file.path().extension() != ".json") continue;
                    Beatmap beatmap(file.path().filename().string(), entry.path().string());
                    beatmap_list.push_back(beatmap);
                }
            }
            // check if loaded, push to beatmap list
            for (const auto& item: beatmaps) {
                if (item.is_load_finished() == SUCCESS) {
                    if (item.id == 0) continue;
                    beatmaps.push_back(item);
                }
            }
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
                    return res ^ ascending;
                });
            }
            // end
            load_finished = true;
        });
        t.detach();
    }

    bool BeatmapLoader::wait_for_scan_finished() {
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