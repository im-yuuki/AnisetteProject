//
// Created by Yuuki on 19/02/2025.
//
#pragma once
#include "data.h"
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_ttf.h>
#include <cstdint>
#include <functional>
#include <string>

namespace anisette::core::abstract {
    class Screen {
    public:
        virtual ~Screen() = default;
        virtual void on_event(const uint64_t &now, const SDL_Event &event) = 0;
        virtual void on_focus(const uint64_t &now) = 0;
        virtual void update(const uint64_t &now) = 0;
    protected:
        SDL_Renderer *renderer = nullptr;
    };
} // namespace anisette::core::abstract

/**
 * @brief Anisette game core, the soul of this project
 *
 * @author Yuuki
 * @version 1.0.0
 */
namespace anisette::core
{
    const uint64_t system_freq = SDL_GetPerformanceFrequency();
    extern data::BeatmapLoader *beatmap_loader;
    extern uint64_t last_frame_time;

    extern void reload_config();

    /**
     * @brief Insert a new frame handler to the core stack
     *
     * The core will redirect the frame update to the handler on the top of the stack.
     *
     * @param handler The frame handler to be inserted
     */
    extern void open(abstract::Screen *handler);

    /**
     * @brief Remove the top frame handler from the core stack
     * 
     * The core will redirect the frame update to the new top handler of the stack.
     */
    extern void back();

    /**
     * @brief Start the game core
     *
     * This function initializes the game core and starts the main game loop.
     * It processes command-line arguments and sets up necessary resources.
     *
     * @return Exit code, 0 for success, otherwise errors
     */
    extern int run();

    /**
     * @brief Request the game core to stop
     */
    extern void request_stop();

    /**
     *
     * @param reg_fn
     */
    extern void register_first_screen(const std::function<abstract::Screen*(SDL_Renderer*)> &reg_fn);

    extern void load_background(const std::string &path, const uint64_t &now);
    extern void toggle_background_parallax(bool enable);
} // namespace anisette::core


/**
 * @brief Handler for rendering and displaying task
 */
namespace anisette::core::video
{
    extern SDL_Renderer *renderer;
    extern SDL_DisplayMode display_mode;
    extern SDL_Rect render_rect;

    extern TTF_Font *primary_font;
    extern TTF_Font *secondary_font;

    enum RenderPositionX { LEFT, CENTER, RIGHT };
    enum RenderPositionY { TOP, MIDDLE, BOTTOM };
}
/**
 * @brief Handler for audio output
 */
namespace anisette::core::audio
{
    extern uint32_t MUSIC_FINISHED_EVENT_ID;
    extern std::string music_display_name;
    extern std::string music_path;
    extern int music_duration_ms;

    [[nodiscard]]
    extern int music_position_ms();
    [[nodiscard]]
    extern uint8_t music_volume();
    [[nodiscard]]
    extern uint8_t sound_volume();
    [[nodiscard]]
    extern bool is_paused();

    extern void set_sound_volume(uint8_t volume);
    extern void set_music_volume(uint8_t volume);

    [[nodiscard]]
    extern Mix_Chunk *load_sound(const std::string &path);
    extern bool play_sound(Mix_Chunk *sound, int channel = -1);

    extern bool play_music(const std::string &path, const std::string &display_name = "");
    extern void pause_music();
    extern void resume_music();
    extern void stop_music();
    extern void seek_music(int position_ms);

    // for quick launch sound
    void play_click_sound();
    void play_hit_sound();
} // namespace anisette::core::audio
