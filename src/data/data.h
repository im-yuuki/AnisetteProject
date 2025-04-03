//
// Created by Yuuki on 02/04/2025.
//
#pragma once
#include <atomic>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>

namespace anisette::data {
    typedef enum {
        NOT_FINISHED,
        SUCCESS,
        FAILED
    } AsyncLoadResult;

    typedef enum {
        BY_ID,
        BY_TITLE,
        BY_ARTIST,
        BY_DIFFICULTY,
        NONE
    } SortStrategy;

    struct Note {
        unsigned start;
       	unsigned end;
    };

    class Beatmap {
    public:
        bool load(const std::string &filename, const std::string &dir);

        unsigned id = 0;
        std::string path;
        std::string artist;
        std::string title;
        std::string thumbnail_path;
        std::string music_path;
        unsigned preview_point = 0;
        uint8_t difficulty = 0;
        uint8_t hp_drain = 0;
        std::vector<Note> notes[6];
    };

    class BeatmapLoader {
    public:
        void scan(SortStrategy sort_strategy, bool ascending = true);
        bool is_scan_finished();
        Beatmap* get_beatmap(int id);
        std::unordered_map<int, int> index;
        std::vector<Beatmap> beatmaps;

    private:
        std::atomic_bool load_finished;
    };
}