//
// Created by Yuuki on 02/04/2025.
//
#pragma once
#include <string>

namespace anisette::data {
    class Beatmap {
    public:
        Beatmap(const std::string &path);
        ~Beatmap();

        void load();
        void unload();


    private:
        std::string path;
        std::string name;
        std::string artist;
        std::string title;
    };
}