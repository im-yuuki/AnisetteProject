//
// Created by Yuuki on 19/02/2025.
//
#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include "abs.h"

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>

/**
 * @brief Anisette game core, the soul of this project
 *
 * @author Yuuki
 * @version 1.0.0
 */
namespace anisette::core
{
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
    enum RenderPositionX { LEFT, CENTER, RIGHT };
    enum RenderPositionY { TOP, MIDDLE, BOTTOM };

    [[nodiscard]]
    extern SDL_Rect get_overlay_render_position(RenderPositionX position_x, RenderPositionY position_y, int width, int height, int margin_x, int margin_y);
}
/**
 * @brief Handler for audio output
 */
namespace anisette::core::audio
{
    struct CurrentMusicInfo {
        std::string display_name;
        std::string path;
        int current_position_ms = 0;
        int total_duration_ms = 0;
        bool paused = false;
    };

    extern void set_sound_volume(uint8_t volume);
    extern void set_music_volume(uint8_t volume);

    [[nodiscard]]
    extern Mix_Chunk *load_sound(const std::string &path);
    extern bool play_sound(Mix_Chunk *sound, int channel = -1);

    [[nodiscard]]
    extern CurrentMusicInfo get_current_music_info();

    extern bool play_music(const std::string &path, const std::string &display_name = "");
    extern void pause_music();
    extern void resume_music();
    extern void stop_music();
} // namespace anisette::core::audio
