//
// Created by Yuuki on 02/04/2025.
//
#pragma once
#include <string>
#include <cstdint>
#include <vector>

namespace anisette::data {
    struct Note {
        unsigned start;
       	unsigned end;
    };

    class Beatmap {
    public:
        Beatmap(const std::string &path);
        ~Beatmap();

        void load();
        void unload();

    private:
        int id;
        std::string artist;
        std::string title;
        std::string thumbnail_path;
        std::string music_path;
        unsigned preview_point;
        uint8_t difficulty;
        uint8_t hp_drain;
        std::vector<Note>* channel[6];
    };

    class BeatmapLoader {

    }
}