//
// Created by Yuuki on 22/03/2025.
//
#include "screens.h"
#include "discord.h"

namespace anisette::screens
{
    LibraryScreen::LibraryScreen(SDL_Renderer *renderer) {
        this->renderer = renderer;
        back_btn      = new components::IconButton("assets/icons/back.png", BTN_TRANSPARENT_COLOR, BTN_HOVER_COLOR);
        arr_left_btn  = new components::IconButton("assets/icons/arrow_left.png", BTN_TRANSPARENT_COLOR, BTN_HOVER_COLOR);
        arr_right_btn = new components::IconButton("assets/icons/arrow_right.png", BTN_TRANSPARENT_COLOR, BTN_HOVER_COLOR);
        play_btn      = new components::TextButton("Go!", 36, BTN_TEXT_COLOR, BTN_BG_COLOR, BTN_HOVER_COLOR);
        top_bar       = new components::HorizontalBox(2, 2);
        body          = new components::HorizontalBox(0, 5);
        bottom_bar    = new components::HorizontalBox(10, 5);

        top_bar->fill_color = BTN_BG_COLOR;
        top_bar->add_item(new components::ItemWrapper(back_btn), 5);
        top_bar->add_item(new components::BlankContainer(), 0);
        body->add_item(new components::ItemWrapper(arr_left_btn), 10);
        body->add_item(new components::BlankContainer(), 0);
        body->add_item(new components::ItemWrapper(arr_right_btn), 10);
        bottom_bar->add_item(new components::ItemWrapper(play_btn), 10);

        main_layout.add_item(top_bar, 5);
        main_layout.add_item(body, 0);
        main_layout.add_item(bottom_bar, 10);
    }

    void LibraryScreen::on_focus(const uint64_t &now) {
        utils::discord::set_browsing_library();
    }

    void LibraryScreen::update(const uint64_t &now) {
        main_layout.draw(renderer, core::video::render_rect);
    }

    void LibraryScreen::on_event(const uint64_t &now, const SDL_Event &event) {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            core::audio::play_click_sound();
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if (utils::check_point_in_rect(x, y, back_btn->last_area)) core::back();
        }
    }

    LibraryScreen::~LibraryScreen() {

    }

}