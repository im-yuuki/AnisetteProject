//
// Created by Yuuki on 22/03/2025.
//
#include "screens.h"
#include "discord.h"
#include "logging.h"
#include <algorithm>

const static auto logger = anisette::logging::get("library");

constexpr int COLOR_RANGE = 4;
constexpr static SDL_Color diff_color[COLOR_RANGE] = {
    {0, 103, 165, 255}, // blue (#0067A5)
    {0, 168, 107, 255}, // green (#00A86B)
    {255, 191, 0, 255}, // yellow (#FFBF00)
    {240, 73, 35, 255}, // red (#F04923)
};
constexpr static int diff_color_bound[COLOR_RANGE] = {0, 20, 40, 60};

namespace anisette::screens
{
    components::Container* create_beatmap_info_view(const data::Beatmap* beatmap) {
        using namespace anisette::components;
        if (!beatmap) return nullptr;
        const uint64_t note_count = beatmap->single_note_count + beatmap->hold_note_count;
        // header
        const auto thumbnail_img = new Image(beatmap->thumbnail_path);
        const auto title_text = new Text(beatmap->title, 24, BTN_TEXT_COLOR);
        title_text->font = core::video::primary_font;
        // labels
        const auto artist_label = new Text("Artist:", 20, BTN_TEXT_COLOR);
        const auto difficulty_label = new Text("Difficulty:", 20, BTN_TEXT_COLOR);
        const auto note_count_label = new Text("Notes:", 20, BTN_TEXT_COLOR);
        // texts
        const auto artist_text = new Text(beatmap->artist, 16, BTN_TEXT_COLOR);
        const auto difficulty_text = new Text(std::to_string(beatmap->difficulty), 16, BTN_TEXT_COLOR);
        const auto note_count_text = new Text(std::to_string(note_count), 16, BTN_TEXT_COLOR);
        // label
        const auto label_vbox = new VerticalBox(0, 2);
        label_vbox->add_item(new ItemWrapper(artist_label))
                  ->add_item(new ItemWrapper(difficulty_label))
                  ->add_item(new ItemWrapper(note_count_label));
        const auto text_vbox = new VerticalBox(0, 2);
        text_vbox->add_item(new ItemWrapper(artist_text))
                 ->add_item(new ItemWrapper(difficulty_text))
                 ->add_item(new ItemWrapper(note_count_text));
        // main vbox
        const auto vbox = new VerticalBox(10, 2);
        vbox->add_item(new ItemWrapper(thumbnail_img), 50);
        vbox->add_item(new ItemWrapper(title_text), 0);
        vbox->add_item((new HorizontalBox(0, 2))->add_item(label_vbox)->add_item(text_vbox), 35);
        // set color for vbox based on difficulty
        const auto color_idx = std::lower_bound(diff_color_bound, diff_color_bound + COLOR_RANGE, beatmap->difficulty);
        const auto color = (color_idx == diff_color_bound + COLOR_RANGE ? diff_color[COLOR_RANGE - 1] : diff_color[color_idx - diff_color_bound]);
        vbox->fill_color = color;
        vbox->fill_rounded_corners = true;
        logger->debug("Loaded beatmap info view: ID {} - Title: {}", beatmap->id, beatmap->title);
        return vbox;
    }

    LibraryScreen::LibraryScreen(SDL_Renderer *renderer) {
        using namespace components;
        this->renderer = renderer;
        for (int i = 0; i < 5; i++) view_wrapper[i] = new ContainerWrapper();

        const auto logo_icon = new Image("assets/icons/app.png");
        const auto title_text = new Text("Select a song", 24, BTN_TEXT_COLOR);

        back_btn      = new IconButton("assets/icons/back.png", BTN_TRANSPARENT_COLOR, BTN_HOVER_COLOR);
        arr_left_btn  = new IconButton("assets/icons/arrow_left.png", BTN_BG_COLOR, BTN_HOVER_COLOR);
        arr_right_btn = new IconButton("assets/icons/arrow_right.png", BTN_BG_COLOR, BTN_HOVER_COLOR);
        play_btn      = new TextButton("Go!", 36, BTN_TEXT_COLOR, BTN_BG_COLOR, BTN_HOVER_COLOR);
        top_bar       = new HorizontalBox(2, 2);
        body          = new HorizontalBox(0, 2);
        bottom_bar    = new HorizontalBox(10, 5);

        top_bar->fill_color = BTN_BG_COLOR;
        top_bar->add_item(new ItemWrapper(back_btn), 5);
        top_bar->add_item(new ItemWrapper(title_text), 0);
        top_bar->add_item(new ItemWrapper(logo_icon), 5);

        body->add_item(new BlankContainer(), 5);
        body->add_item((new VerticalBox())->add_item(view_wrapper[0], 90));
        body->add_item((new VerticalBox())->add_item(view_wrapper[1], 95));
        body->add_item((new VerticalBox())->add_item(view_wrapper[2], 100));
        body->add_item((new VerticalBox())->add_item(view_wrapper[3], 95));
        body->add_item((new VerticalBox())->add_item(view_wrapper[4], 90));
        body->add_item(new BlankContainer(), 5);

        bottom_bar->add_item(new BlankContainer(), 0);
        bottom_bar->add_item(new ItemWrapper(arr_left_btn), 10);
        bottom_bar->add_item(new ItemWrapper(play_btn), 20);
        bottom_bar->add_item(new ItemWrapper(arr_right_btn), 10);
        bottom_bar->add_item(new BlankContainer(), 0);

        main_layout.add_item(top_bar, 5);
        main_layout.add_item(new BlankContainer(), 0);
        main_layout.add_item(body, 50);
        main_layout.add_item(new BlankContainer(), 0);
        main_layout.add_item(bottom_bar, 10);

        // load view
        beatmap_view.clear();
        beatmap_view.emplace_back();
        beatmap_view.emplace_back();
        for (int i = 0; i < 3; i++) {
            if (i >= core::beatmap_loader->beatmaps.size()) {
                beatmap_view.emplace_back();
                continue;
            }
            beatmap_view.emplace_back(i, &core::beatmap_loader->beatmaps[i]);
            view_wrapper[i + 2]->set_back_container(beatmap_view.at(i + 2).view);
        }
        // action hooks
        action_start_time = SDL_GetPerformanceCounter();
    }

    void LibraryScreen::on_focus(const uint64_t &now) {
        utils::discord::set_browsing_library();
        if (action_hook.empty()) action_start_time = now;
        // fade in
        if (action_hook.empty()) action_start_time = now;
        action_hook.emplace([this](const uint64_t &action_now) {
            const auto delta = action_now > action_start_time ? action_now - action_start_time : 0;
            const auto alpha = 255 * delta / fade_duration;
            if (alpha > 255) {
                screen_dim_alpha = 0;
                logger->debug("Fade in finished");
                return true;
            }
            screen_dim_alpha = 255 - alpha;
            return false;
        });
        if (action_hook.empty()) action_start_time = now;
        action_hook.emplace([this](const uint64_t &action_now) {
            reload_selected_beatmap(action_now);
            return true;
        });
    }

    void LibraryScreen::update(const uint64_t &now) {
        // draw main layout
        if (screen_dim_alpha < 255) main_layout.draw(renderer, core::video::render_rect);

        // dim screen
        if (screen_dim_alpha > 0) {
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, screen_dim_alpha);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_RenderFillRect(renderer, &core::video::render_rect);
        }

        // run action hooks
        if (!action_hook.empty()) if (action_hook.front()(now)) {
            action_hook.pop();
            action_start_time = now;
        }

    }

    void LibraryScreen::on_event(const uint64_t &now, const SDL_Event &event) {
        if (event.type == core::audio::MUSIC_FINISHED_EVENT_ID) {
            const auto current_beatmap = beatmap_view[2].beatmap;
            if (!current_beatmap) {
                logger->warn("No beatmap selected");
                return;
            }
            core::audio::play_music(current_beatmap->music_path, current_beatmap->title + " - " + current_beatmap->artist);
            core::audio::seek_music(current_beatmap->preview_point);
        } else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_KEYDOWN) {
            core::audio::play_click_sound();
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if (utils::check_point_in_rect(x, y, back_btn->last_area)) {
                if (action_hook.empty()) action_start_time = now;
                action_hook.emplace([this](const uint64_t &action_now) {
                    const auto delta = action_now > action_start_time ? action_now - action_start_time : 0;
                    const auto alpha = 255 * delta / fade_duration;
                    if (alpha > 255) {
                        screen_dim_alpha = 255;
                        logger->debug("Fade out finished");
                        core::back();
                        return true;
                    }
                    screen_dim_alpha = alpha;
                    return false;
                });
            } else if (utils::check_point_in_rect(x, y, play_btn->last_area)) {
                logger->warn("Play button is not implemented");
            } else if (utils::check_point_in_rect(x, y, arr_left_btn->last_area)) {
                prev_beatmap(now);
            } else if (utils::check_point_in_rect(x, y, arr_right_btn->last_area)) {
                next_beatmap(now);
            }
        } else if (event.type == SDL_KEYUP) {
            const auto key = event.key.keysym.sym;
            if (key == SDLK_ESCAPE) {
                if (action_hook.empty()) action_start_time = now;
                action_hook.emplace([this](const uint64_t &action_now) {
                    const auto delta = action_now > action_start_time ? action_now - action_start_time : 0;
                    const auto alpha = 255 * delta / fade_duration;
                    if (alpha > 255) {
                        screen_dim_alpha = 255;
                        logger->debug("Fade out finished");
                        core::back();
                        return true;
                    }
                    screen_dim_alpha = alpha;
                    return false;
                });
            } else if (key == SDLK_RETURN) {
                logger->warn("Play button is not implemented");
            } else if (key == SDLK_LEFT) {
                prev_beatmap(now);
            } else if (key == SDLK_RIGHT) {
                next_beatmap(now);
            }
        }
    }

    void LibraryScreen::prev_beatmap(const uint64_t &now) {
        if (selected_song_index == 0) return;
        selected_song_index--;
        beatmap_view.pop_back();
        if (selected_song_index - 2 < 0) beatmap_view.emplace_front();
        else beatmap_view.emplace_front(selected_song_index - 2, &core::beatmap_loader->beatmaps[selected_song_index - 2]);
        for (int i = 0; i < 5; i++) view_wrapper[i]->set_back_container(beatmap_view.at(i).view);
        if (action_hook.empty()) action_start_time = now;
        action_hook.emplace([this](const uint64_t &action_now) {
            reload_selected_beatmap(action_now);
            return true;
        });
    }

    void LibraryScreen::next_beatmap(const uint64_t &now) {
        if (selected_song_index == core::beatmap_loader->beatmaps.size() - 1) return;
        selected_song_index++;
        beatmap_view.pop_front();
        if (selected_song_index + 2 >= core::beatmap_loader->beatmaps.size()) beatmap_view.emplace_back();
        else beatmap_view.emplace_back(selected_song_index + 2, &core::beatmap_loader->beatmaps[selected_song_index + 2]);
        for (int i = 0; i < 5; i++) view_wrapper[i]->set_back_container(beatmap_view.at(i).view);
        if (action_hook.empty()) action_start_time = now;
        action_hook.emplace([this](const uint64_t &action_now) {
            reload_selected_beatmap(action_now);
            return true;
        });
    }

    void LibraryScreen::reload_selected_beatmap(const uint64_t &now) const {
        const auto current_beatmap = beatmap_view[2].beatmap;
        if (!current_beatmap) {
            logger->warn("No beatmap selected");
            return;
        }
        logger->debug("Selected beatmap ID: {}", current_beatmap->id);
        if (beatmap_view[2].beatmap->music_path.empty()) {
            logger->warn("Beatmap ID {} has no music path", current_beatmap->id);
        } else if (core::audio::music_path != current_beatmap->music_path) {
            core::audio::play_music(current_beatmap->music_path, current_beatmap->title + " - " + current_beatmap->artist);
            core::audio::seek_music(current_beatmap->preview_point);
        }
        if (current_beatmap->thumbnail_path.empty()) {
            logger->warn("Beatmap ID {} has no thumbnail path", current_beatmap->id);
        } else {
            core::load_background(current_beatmap->thumbnail_path, now);
        }
    }

    LibraryScreen::~LibraryScreen() {}

    LibraryScreen::BeatmapViewItem::BeatmapViewItem(const int index, data::Beatmap *beatmap) {
        this->index = index;
        this->beatmap = beatmap;
        view = create_beatmap_info_view(beatmap);
    }

     LibraryScreen::BeatmapViewItem::~BeatmapViewItem() {
        delete view;
    }

}