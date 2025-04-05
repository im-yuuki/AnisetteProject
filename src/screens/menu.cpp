//
// Created by Yuuki on 22/03/2025.
//
#include <filesystem>
#include "screens.h"
#include "discord.h"
#include "core.h"
#include "logging.h"
#include "container.h"
#include <SDL2/SDL_events.h>

#define VOLUME_CHANGE_STEP 8

const auto logger = anisette::logging::get("menu");

namespace anisette::screens
{
    MenuScreen::MenuScreen(SDL_Renderer *renderer) : renderer(renderer) {
        using namespace components;
        // add elements to the grid
        auto logo = new Image("assets/logo.png");
        auto play_btn = new TextButton("Play!", 24, {255, 255, 255, 255}, {0, 0, 0, 128}, {100, 100, 100, 255});
        auto settings_btn = new TextButton("Settings", 20, {255, 255, 255, 255}, {0, 0, 0, 128}, {100, 100, 100, 255});
        auto quit_btn = new TextButton("Exit", 20, {255, 255, 255, 255}, {0, 0, 0, 128}, {100, 100, 100, 255});
        auto vbox = new VerticalBox(2, 2);
        vbox->add_item(new ItemWrapper(logo), 60);
        vbox->add_item(new ItemWrapper(play_btn), 0);
        auto hbox = new HorizontalBox(0, 2);
        hbox->add_item(new ItemWrapper(settings_btn), 0);
        hbox->add_item(new ItemWrapper(quit_btn), 0);
        vbox->add_item(hbox, 0);
        grid.add_child(vbox, GridChildProperties::CENTER, GridChildProperties::MIDDLE, 80, 50);
        // add hook to play music from a random beatmap
        action_hook.emplace([this](const uint64_t &now) {
            play_random_music();
            return true;
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
    }

    void MenuScreen::update(const uint64_t &now) {
        if (!load_async_finshed) return;

        // draw the grid
        grid.draw(renderer, core::video::render_rect, 255);

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
                core::audio::play_click_sound();
                return;
            case SDL_MOUSEBUTTONUP:
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                on_click(now, mouse_x, mouse_y);
                return;
            case SDL_MOUSEWHEEL:
                if (event.wheel.y > 0) {
                    // increase music volume
                    core::audio::set_music_volume(core::audio::music_volume() + VOLUME_CHANGE_STEP);
                } else if (event.wheel.y < 0) {
                    // decrease music volume
                    core::audio::set_music_volume(core::audio::music_volume() > VOLUME_CHANGE_STEP ? core::audio::music_volume() - VOLUME_CHANGE_STEP : 0);
                }
            default:
                break;
        }
    }

    void MenuScreen::load_async() {
        std::thread t([this]() {
            logger->debug("Loading menu screen");
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
            load_async_finshed = true;
        });
        t.detach();
    }

    bool MenuScreen::is_load_async_finished() {
        return load_async_finshed;
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

} // namespace anisette::screens
