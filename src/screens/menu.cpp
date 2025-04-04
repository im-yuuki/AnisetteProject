//
// Created by Yuuki on 22/03/2025.
//
#include <filesystem>
#include "screens.h"
#include "discord.h"
#include "core.h"
#include "logging.h"

#include <SDL2/SDL_events.h>

const auto logger = anisette::logging::get("menu");

namespace anisette::screens
{
    MenuScreen::MenuScreen(SDL_Renderer *renderer) : renderer(renderer) {
        // add hook to play music from a random beatmap
        action_hook.push([this](const uint64_t &now) {
            return play_random_music();
        });
    }

    bool MenuScreen::play_random_music() {
        if (core::beatmap_loader->beatmaps.empty()) {
            logger->error("No beatmaps found");
            return false;
        }
        logger->debug("Play random music");
        const auto beatmap = core::beatmap_loader->beatmaps[utils::randint(0, core::beatmap_loader->beatmaps.size() - 1)];
        const auto music_path = beatmap.music_path;
        const auto display_name = beatmap.artist + " - " + beatmap.title;
        core::audio::play_music(music_path, display_name);
        return true;
    }

    void MenuScreen::on_click(const uint64_t &now, const int x, const int y) {
        // TODO: implement menu click actions
    }

    void MenuScreen::update(const uint64_t &now) {
        if (!load_async_finished) return;

        // run action hooks
        if (!action_hook.empty()) if (action_hook.front()(now)) {
            action_hook.pop();
            action_start_time = now;
        }
    }

    void MenuScreen::on_event(const uint64_t &now, const SDL_Event &event) {
        if (event.type == core::audio::MUSIC_FINISHED_EVENT_ID) {
            play_random_music();
        }
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                core::audio::play_sound(click_sound, 1);
                return;
            case SDL_MOUSEBUTTONUP:
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                on_click(now, mouse_x, mouse_y);
                return;
            case SDL_MOUSEWHEEL:
                return;
            default:
                break;
        }
    }

    void MenuScreen::load_async() {
        std::thread t([this]() {
            logger->debug("Loading menu screen");
            // load the click sound
            click_sound = core::audio::load_sound("assets/sound/click.wav");
            if (!click_sound) {
                logger->error("Failed to load click sound");
                return;
            }
            // choose a random background
            for (const auto &entry: std::filesystem::directory_iterator("assets/backgrounds")) {
                if (entry.is_regular_file()) {
                    // allowed: .jpg .jpeg .png .bmp
                    if (entry.path().extension() == ".jpg") goto accept;
                    if (entry.path().extension() == ".jpeg") goto accept;
                    if (entry.path().extension() == ".png") goto accept;
                    if (entry.path().extension() == ".bmp") goto accept;
                    continue;

                    accept:
                    default_backgrounds.push_back(entry.path().string());
                }
            }
            load_async_finished = true;
        });
        t.detach();
    }

    bool MenuScreen::is_load_async_finished() {
        return load_async_finished;
    }

    void MenuScreen::on_focus(const uint64_t &now) {
        utils::discord::set_in_main_menu();
        // choose a random background
        if (default_backgrounds.empty()) {
            logger->error("No background found");
            return;
        }
        const auto random_index = utils::randint(0, default_backgrounds.size() - 1);
        logger->debug("Load background: {}", default_backgrounds[random_index]);
        core::load_background(default_backgrounds[random_index], now);
        core::toggle_background_parallax(true);
    }

    MenuScreen::~MenuScreen() {
        Mix_FreeChunk(click_sound);
    }

} // namespace anisette::screens
