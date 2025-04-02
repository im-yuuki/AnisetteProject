//
// Created by Yuuki on 22/03/2025.
//

#include <SDL_image.h>
#include <cassert>
#include <filesystem>
#include <register.h>
#include "core/core.h"
#include "utils/discord.h"
#include "utils/logging.h"

const auto logger = anisette::logging::get("menu");

namespace anisette::screens
{
    MenuScreen::MenuScreen(SDL_Renderer *renderer) : renderer(renderer) {
        click_sound = core::audio::load_sound("assets/sound/click.wav");
        display_mode = core::video::get_display_mode();
        // choose a random background
        std::vector<std::string> backgrounds;
        for (const auto &entry: std::filesystem::directory_iterator("assets/backgrounds")) {
            if (entry.is_regular_file()) {
                // check if the file is a jpg file
                if (entry.path().extension() != ".jpg")
                    continue;
                backgrounds.push_back(entry.path().string());
            }
        }
        assert(!backgrounds.empty());
        const auto random_index = rand() % backgrounds.size();
        auto bg_surface = IMG_Load(backgrounds[random_index].c_str());
        assert(bg_surface);
        // scale the image to fit the screen
        const float image_ratio = static_cast<float>(bg_surface->w) / bg_surface->h;
        const float screen_ratio = static_cast<float>(display_mode->w) / display_mode->h;
        if (image_ratio > screen_ratio) {
            bg_src_rect.w = bg_surface->w - parallax_range;
            bg_src_rect.h = bg_surface->w / screen_ratio - parallax_range;
        } else {
            bg_src_rect.h = bg_surface->h - parallax_range;
            bg_src_rect.w = bg_surface->h * screen_ratio - parallax_range;
        }
        background = SDL_CreateTextureFromSurface(renderer, bg_surface);
        SDL_FreeSurface(bg_surface);
        assert(background);
        utils::discord::set_in_main_menu();
    }

    void MenuScreen::update(const uint64_t &now) {
        // get mouse position
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        // calculate parallax offset
        bg_src_rect.x = parallax_range - mouse_x * parallax_range/ display_mode->w;
        bg_src_rect.y = parallax_range - mouse_y * parallax_range / display_mode->h;

        // parallax offset
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, &bg_src_rect, nullptr);
    }

    void MenuScreen::on_event(const uint64_t &now, const SDL_Event &event) {
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                core::audio::play_sound(click_sound);
            case SDL_MOUSEBUTTONUP:

            default:
                break;
        }
    }

    MenuScreen::~MenuScreen() {
        SDL_DestroyTexture(background);
        Mix_FreeChunk(click_sound);
    }

} // namespace anisette::screens
