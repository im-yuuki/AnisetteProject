//
// Created by Yuuki on 22/03/2025.
//

#include <SDL_image.h>
#include <core/_internal.h>
#include <filesystem>
#include <register.h>

#include "core/core.h"
#include "utils/discord.h"
#include "utils/logging.h"

const auto logger = anisette::logging::get("menu");

namespace anisette::screens
{
    MenuScreen::MenuScreen(SDL_Renderer *renderer) : renderer(renderer) {
        display_mode = core::video::get_display_mode();
    }

    void MenuScreen::on_click(const int x, const int y) {

    }

    void MenuScreen::update(const uint64_t &now) {
        if (!load_async_finished) return;

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

        // run action hooks
        if (!action_hook.empty()) if (action_hook.front()(now)) {
            action_hook.pop();
            action_start_time = now;
        }
    }

    void MenuScreen::on_event(const uint64_t &now, const SDL_Event &event) {
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                core::audio::play_sound(click_sound, 1);
                return;
            case SDL_MOUSEBUTTONUP:
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                on_click(mouse_x, mouse_y);
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
            std::vector<std::string> backgrounds;
            for (const auto &entry: std::filesystem::directory_iterator("assets/backgrounds")) {
                if (entry.is_regular_file()) {
                    // allowed: .jpg .jpeg .png .bmp
                    if (entry.path().extension() == ".jpg") goto accept;
                    if (entry.path().extension() == ".jpeg") goto accept;
                    if (entry.path().extension() == ".png") goto accept;
                    if (entry.path().extension() == ".bmp") goto accept;
                    continue;

                    accept:
                    backgrounds.push_back(entry.path().string());
                }
            }
            if (backgrounds.empty()) {
                logger->error("No backgrounds found");
                return;
            }
            const auto random_index = utils::randint(0, backgrounds.size() - 1);
            // load the background image
            background_surface = IMG_Load(backgrounds[random_index].c_str());
            const int width = background_surface->w, height = background_surface->h;
            // scale the image to fit the screen
            const float image_ratio = static_cast<float>(width) / height;
            const float screen_ratio = static_cast<float>(display_mode->w) / display_mode->h;
            if (image_ratio > screen_ratio) {
                bg_src_rect.w = width - parallax_range;
                bg_src_rect.h = width / screen_ratio - parallax_range;
            } else {
                bg_src_rect.h = width - parallax_range;
                bg_src_rect.w = width * screen_ratio - parallax_range;
            }
            load_async_finished = true;
        });
        t.detach();
    }

    bool MenuScreen::is_load_async_finished() {
        if (!load_async_finished) return false;
        // load the background image
        if (background_surface) {
            SDL_DestroyTexture(background);
            background = SDL_CreateTextureFromSurface(renderer, background_surface);
            SDL_SetTextureBlendMode(background, SDL_BLENDMODE_BLEND);
            SDL_FreeSurface(background_surface);
            background_surface = nullptr;
        }
        return true;
    }

    void MenuScreen::on_focus(const uint64_t &now) {
        utils::discord::set_in_main_menu();
        SDL_SetTextureAlphaMod(background, 0);
        action_start_time = now;
        action_hook.emplace([this](const uint64_t &_now) {
            const auto alpha = 255 * (_now - action_start_time) / utils::system_freq;
            if (alpha > 255) {
                SDL_SetTextureAlphaMod(background, 255);
                return true;
            }
            SDL_SetTextureAlphaMod(background, alpha);
            return false;
        });
    }

    MenuScreen::~MenuScreen() {
        SDL_DestroyTexture(background);
        Mix_FreeChunk(click_sound);
    }

} // namespace anisette::screens
