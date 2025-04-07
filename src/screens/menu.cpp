//
// Created by Yuuki on 22/03/2025.
//
#include <filesystem>
#include <SDL2/SDL_events.h>
#include "container.h"
#include "core.h"
#include "discord.h"
#include "logging.h"
#include "screens.h"

#define VOLUME_CHANGE_STEP 4

const auto logger = anisette::logging::get("menu");

namespace anisette::screens
{
    MenuScreen::MenuScreen(SDL_Renderer *renderer) {
        using namespace components;
        this->renderer = renderer;

        // create buttons
        play_btn        = new TextButton("Play!", 24, BTN_TEXT_COLOR, BTN_BG_COLOR, BTN_HOVER_COLOR);
        settings_btn    = new TextButton("Settings", 20, BTN_TEXT_COLOR, BTN_BG_COLOR, BTN_HOVER_COLOR);
        quit_btn        = new TextButton("Exit", 20, BTN_TEXT_COLOR, BTN_BG_COLOR, BTN_HOVER_COLOR);

        const auto vbox = new VerticalBox(2, 2);
        vbox->add_item(new ItemWrapper(new Image("assets/logo.png")), 60);
        vbox->add_item(new ItemWrapper(play_btn), 0);
        vbox->add_item(new ItemWrapper(settings_btn), 0);
        vbox->add_item(new ItemWrapper(quit_btn), 0);

        // music control
        now_playing_text = new Text("", 24, BTN_TEXT_COLOR);
        music_play_btn   = new IconButton("assets/icons/play.png", BTN_BG_COLOR, BTN_HOVER_COLOR);
        music_pause_btn  = new IconButton("assets/icons/pause.png", BTN_BG_COLOR, BTN_HOVER_COLOR);
        music_stop_btn   = new IconButton("assets/icons/stop.png", BTN_BG_COLOR, BTN_HOVER_COLOR);
        music_next_btn   = new IconButton("assets/icons/next.png", BTN_BG_COLOR, BTN_HOVER_COLOR);
        music_prev_btn   = new IconButton("assets/icons/previous.png", BTN_BG_COLOR, BTN_HOVER_COLOR);

        music_play_btn_wrapper  = new ItemWrapper(music_play_btn);
        music_pause_btn_wrapper = new ItemWrapper(music_pause_btn);

        const auto music_control = new HorizontalBox(0, 2);
        music_control->add_item(new ItemWrapper(now_playing_text), 70);
        music_control->add_item(new ItemWrapper(music_prev_btn), 0);
        music_control->add_item(music_play_btn_wrapper, 0);
        music_control->add_item(music_pause_btn_wrapper, 0);
        music_control->add_item(new ItemWrapper(music_next_btn), 0);
        music_control->add_item(new ItemWrapper(music_stop_btn), 0);
        // volume overlay
        volume_mute_icon = new ItemWrapper(new Image("assets/icons/vol_mute.png"));
        volume_low_icon  = new ItemWrapper(new Image("assets/icons/vol_low.png"));
        volume_high_icon = new ItemWrapper(new Image("assets/icons/vol_high.png"));
        volume_text      = new Text("", 20, BTN_TEXT_COLOR);
        volume_bar       = new ProgressBar(128, {235, 0, 85, 255}, {255, 255, 255, 128});
        volume_mute_icon->set_hidden(true);
        volume_mute_icon->set_hidden(true);
        volume_high_icon->set_hidden(true);

        volume_overlay = new HorizontalBox(0, 2);
        volume_overlay->fill_color = BTN_BG_COLOR;
        volume_overlay->add_item(volume_mute_icon, 0);
        volume_overlay->add_item(volume_low_icon, 0);
        volume_overlay->add_item(volume_high_icon, 0);
        volume_overlay->add_item(new ItemWrapper(volume_text), 0);
        volume_overlay->add_item(new ItemWrapper(volume_bar), 85);
        volume_overlay->set_hidden(true);
        // add elements to the grid
        grid.add_child(vbox, GridChildProperties::CENTER, GridChildProperties::MIDDLE, 80, 50);
        grid.add_child(music_control, GridChildProperties::RIGHT, GridChildProperties::TOP, 70, 4);
        grid.add_child(volume_overlay, GridChildProperties::LEFT, GridChildProperties::BOTTOM, 70, 3);
        // add hook to play music from a random beatmap
        action_hook.emplace([this](const uint64_t &now) {
            play_random_music();
            return true;
        });
    }

    void MenuScreen::play_random_music() const {
        if (core::beatmap_loader->beatmaps.empty()) {
            logger->error("No beatmaps found");
            return;
        }
        logger->debug("Play random music");
        const auto beatmap = core::beatmap_loader->beatmaps[utils::randint(0, core::beatmap_loader->beatmaps.size() - 1)];
        const auto music_path = beatmap.music_path;
        const auto display_name = beatmap.title + " - " + beatmap.artist;
        if (core::audio::play_music(music_path, display_name)) {
            now_playing_text->change_text(display_name);
            music_play_btn_wrapper->set_hidden(true);
            music_pause_btn_wrapper->set_hidden(false);
        } else {
            now_playing_text->change_text("");
            music_play_btn_wrapper->set_hidden(false);
            music_pause_btn_wrapper->set_hidden(true);
        }
    }

    void MenuScreen::on_click(const uint64_t &now, const int x, const int y) const {
        if (utils::check_point_in_rect(x, y, play_btn->last_area)) {
            logger->debug("Clicked play button");
            if (core::beatmap_loader->beatmaps.empty()) logger->warn("No beatmaps found");
            else core::open(new LibraryScreen(renderer));
        } else if (utils::check_point_in_rect(x, y, settings_btn->last_area)) {
            logger->debug("Clicked settings button");
        } else if (utils::check_point_in_rect(x, y, quit_btn->last_area)) {
            logger->debug("Clicked quit button");
            core::request_stop();
        } else if (utils::check_point_in_rect(x, y, music_play_btn->last_area)) {
            logger->debug("Clicked play music button");
            if (core::audio::music_path.empty()) {
                play_random_music();
            } else {
                core::audio::resume_music();
                music_play_btn_wrapper->set_hidden(true);
                music_pause_btn_wrapper->set_hidden(false);
            }
        } else if (utils::check_point_in_rect(x, y, music_pause_btn->last_area)) {
            logger->debug("Clicked pause music button");
            core::audio::pause_music();
            music_play_btn_wrapper->set_hidden(false);
            music_pause_btn_wrapper->set_hidden(true);
        } else if (utils::check_point_in_rect(x, y, music_stop_btn->last_area)) {
            logger->debug("Clicked stop music button");
            core::audio::stop_music();
            music_play_btn_wrapper->set_hidden(false);
            music_pause_btn_wrapper->set_hidden(true);
            now_playing_text->change_text("");
        } else if (utils::check_point_in_rect(x, y, music_next_btn->last_area)) {
            logger->debug("Clicked next music button");
            play_random_music();
        } else if (utils::check_point_in_rect(x, y, music_prev_btn->last_area)) {
            logger->debug("Clicked previous music button");
            core::audio::seek_music(0);
            core::audio::resume_music();
            music_play_btn_wrapper->set_hidden(true);
            music_pause_btn_wrapper->set_hidden(false);
        }
    }

    void MenuScreen::update(const uint64_t &now) {
        if (!load_async_finshed) return;

        // if volume changes, show overlay
        if (volume_overlay_hide_time != 0 && now > volume_overlay_hide_time) volume_overlay->set_hidden(true);

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
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            core::audio::play_click_sound();
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            int mouse_x, mouse_y;
            SDL_GetMouseState(&mouse_x, &mouse_y);
            on_click(now, mouse_x, mouse_y);
        } else if (event.type == SDL_MOUSEWHEEL) {
            uint8_t new_volume = core::audio::music_volume();
            if (event.wheel.y > 0) {
                // increase music volume
                new_volume += VOLUME_CHANGE_STEP;
                if (new_volume > 128) new_volume = 128;
            } else if (event.wheel.y < 0) {
                // decrease music volume
                if (new_volume > VOLUME_CHANGE_STEP) new_volume -= VOLUME_CHANGE_STEP;
                else new_volume = 0;
            }
            core::audio::set_music_volume(new_volume);
            if (new_volume == 0) {
                volume_mute_icon->set_hidden(false);
                volume_low_icon->set_hidden(true);
                volume_high_icon->set_hidden(true);
            } else if (new_volume < 64) {
                volume_mute_icon->set_hidden(true);
                volume_low_icon->set_hidden(false);
                volume_high_icon->set_hidden(true);
            } else {
                volume_mute_icon->set_hidden(true);
                volume_low_icon->set_hidden(true);
                volume_high_icon->set_hidden(false);
            }
            volume_text->change_text(std::to_string(new_volume));
            volume_bar->value = new_volume;
            volume_overlay->set_hidden(false);
            volume_overlay_hide_time = now + core::system_freq * 3; // hide after 3 seconds
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
            // if no beatmaps loaded, disable play button
            while (!core::beatmap_loader->is_scan_finished()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            if (core::beatmap_loader->beatmaps.empty()) {
                play_btn->background = BTN_DISABLED_COLOR;
                play_btn->hover_background = BTN_DISABLED_COLOR;
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
        // reload music state
        music_play_btn_wrapper->set_hidden(core::audio::is_paused());
        music_pause_btn_wrapper->set_hidden(!core::audio::is_paused());
        now_playing_text->change_text(core::audio::music_display_name);
    }
} // namespace anisette::screens
