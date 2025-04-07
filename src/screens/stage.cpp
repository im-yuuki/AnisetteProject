//
// Created by Yuuki on 03/04/2025.
//
#include "screens.h"
#include "discord.h"
#include "logging.h"

// Keymap: Q W E I O P
#define STAGE_TEXT_PRIMARY_SIZE 40
#define STAGE_TEXT_SECONDARY_SIZE 28

const static auto logger = anisette::logging::get("stage");

namespace anisette::screens
{
    StageScreen::StageScreen(SDL_Renderer *renderer, data::Beatmap *beatmap) : beatmap(beatmap) {
        using namespace components;
        this->renderer = renderer;
        logger->debug("Set base offset to {}ms", (100 - beatmap->difficulty) * 2);
        score_calculator = new utils::ScoreCalculator((100 - beatmap->difficulty) * 2, beatmap->hp_drain);
        channel[0] = new StageChannel(score_calculator, &beatmap->notes[0], "Q");
        channel[1] = new StageChannel(score_calculator, &beatmap->notes[1], "W");
        channel[2] = new StageChannel(score_calculator, &beatmap->notes[2], "E");
        channel[3] = new StageChannel(score_calculator, &beatmap->notes[3], "I");
        channel[4] = new StageChannel(score_calculator, &beatmap->notes[4], "O");
        channel[5] = new StageChannel(score_calculator, &beatmap->notes[5], "P");
        // temporary disable 2 channels for easier
        channel[0]->set_hidden(true);
        channel[5]->set_hidden(true);
        // texts
        combo_text = new Text("0x", STAGE_TEXT_PRIMARY_SIZE, BTN_TEXT_COLOR);
        score_text = new Text("00000000", STAGE_TEXT_PRIMARY_SIZE, BTN_TEXT_COLOR);
        accuracy_text = new Text("100.00%", STAGE_TEXT_SECONDARY_SIZE, BTN_TEXT_COLOR);
        // health bar
        const auto heart_icon = new Image("assets/icons/heart.png");
        health_bar = new ProgressBar(100, BTN_HOVER_COLOR, BTN_BG_COLOR);
        const auto health_hbox = new HorizontalBox(0, 2);
        health_hbox->add_item(new ItemWrapper(heart_icon), 15);
        health_hbox->add_item(new ItemWrapper(health_bar));
        // left + right vbox
        const auto left_vbox = new VerticalBox(5, 1);
        left_vbox->add_item(health_hbox, 5);
        left_vbox->add_item(new BlankContainer());
        left_vbox->add_item((new HorizontalBox())->add_item(new ItemWrapper(combo_text))->add_item(new BlankContainer(), 70), 5);

        const auto right_vbox = new VerticalBox(5, 1);
        right_vbox->add_item((new HorizontalBox())->add_item(new BlankContainer(), 60)->add_item(new ItemWrapper(score_text)), 4);
        right_vbox->add_item((new HorizontalBox())->add_item(new BlankContainer(), 70)->add_item(new ItemWrapper(accuracy_text)), 4);
        right_vbox->add_item(new BlankContainer());

        hbox.add_item(left_vbox, 25);
        for (const auto &i : channel) hbox.add_item(i);
        hbox.add_item(right_vbox, 25);
        action_start_time = SDL_GetPerformanceCounter();
    }

    StageScreen::~StageScreen() {
        delete score_calculator;
    }


    void StageScreen::on_event(const uint64_t &now, const SDL_Event &event) {
        if (event.type == SDL_MOUSEBUTTONDOWN) core::audio::play_click_sound();
        else if (event.type == core::audio::MUSIC_FINISHED_EVENT_ID) {
            logger->debug("Finished playing music");
            core::back();
        } else if (event.type == SDL_KEYUP) {
            // exit stage
            if (event.key.keysym.sym == SDLK_ESCAPE) {
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
            }
        }
    }

    void StageScreen::update(const uint64_t &now) {
        // run action hooks
        if (!action_hook.empty()) if (action_hook.front()(now)) {
            action_hook.pop();
            action_start_time = now;
        }
        // if 6 channels finished, stop
        if (channel[0]->finished && channel[1]->finished && channel[2]->finished && channel[3]->finished && channel[4]->finished && channel[5]->finished) {
            logger->debug("All channels finished");
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
        }
        // scan key
        const auto scan_res = SDL_GetKeyboardState(nullptr);
        // bind values
        if (!paused) {
            const auto this_tick = SDL_GetTicks();
            current_music_pos_ms += this_tick - last_tick;
            last_tick = this_tick;
        }
        channel[0]->bind_value(current_music_pos_ms, scan_res[SDL_SCANCODE_Q]);
        channel[1]->bind_value(current_music_pos_ms, scan_res[SDL_SCANCODE_W]);
        channel[2]->bind_value(current_music_pos_ms, scan_res[SDL_SCANCODE_E]);
        channel[3]->bind_value(current_music_pos_ms, scan_res[SDL_SCANCODE_I]);
        channel[4]->bind_value(current_music_pos_ms, scan_res[SDL_SCANCODE_O]);
        channel[5]->bind_value(current_music_pos_ms, scan_res[SDL_SCANCODE_P]);
        // update statistics
        combo_text->change_text(score_calculator->get_combo_string());
        score_text->change_text(score_calculator->get_score_string());
        accuracy_text->change_text(score_calculator->get_accuracy_percentage_string());
        health_bar->value = score_calculator->hp;
        // dim background
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, &core::video::render_rect);
        // draw hbox
        if (screen_dim_alpha < 255) hbox.draw(renderer, core::video::render_rect);
        // dim screen
        if (screen_dim_alpha > 0) {
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, screen_dim_alpha);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_RenderFillRect(renderer, &core::video::render_rect);
        }
    }

    void StageScreen::on_focus(const uint64_t &now) {
        utils::discord::set_playing_song(beatmap->title, beatmap->artist);
        core::toggle_background_parallax(false);
        if (action_hook.empty()) action_start_time = now;
        // load music
        action_hook.emplace([this](const uint64_t &action_now) {
            core::audio::stop_music();
            core::audio::play_music(beatmap->music_path, beatmap->title + " - " + beatmap->artist);
            paused = false;
            // temporary pause
            core::audio::pause_music();
            return true;
        });
        // fade in action
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
        // wait 3s then start music
        action_hook.emplace([this](const uint64_t &action_now) {
            if (current_music_pos_ms >= 0) {
                core::audio::resume_music();
                last_tick = SDL_GetTicks();
                return true;
            }
            return false;
        });
    }
} // namespace anisette::screens
