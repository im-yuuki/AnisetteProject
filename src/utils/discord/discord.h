//
// Created by Yuuki on 16/03/2025.
//
#pragma once

namespace anisette::utils::discord {
    void init();
    void cleanup();
    void update();
    void set_idle();
    void set_in_main_menu();
    void set_browsing_libary();
    void set_playing_song(const char *song, const char* difficulty);
    void set_spectate_song(const char *song, const char* difficulty);
}