//
// Created by Yuuki on 19/02/2025.
//
#pragma once
#include <cstdint>
#include <functional>
#include "abs.h"

/**
 * @brief Anisette game core, the soul of this project
 *
 * @author Yuuki
 * @version 1.0.0
 */
namespace anisette::core
{
    /**
     * @brief Frame time of the latest handle
     */
    inline uint64_t last_frame_time = 0;

    /**
     * @brief Set the frame rate limit
     *
     * @param value FPS
     */
    extern void set_fps(int value);

    /**
     * @brief Insert a new frame handler to the core stack
     *
     * The core will redirect the frame update to the handler on the top of the stack.
     *
     * @param handler The frame handler to be inserted
     */
    extern void insert_handler(abstract::FrameHandler *handler);

    /**
     * @brief Remove the top frame handler from the core stack
     * 
     * The core will redirect the frame update to the new top handler of the stack.
     */
    extern void remove_handler();

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
    extern void register_first_frame_handler(const std::function<abstract::FrameHandler *()> &reg_fn);
} // namespace anisette::core


/**
 * @brief Handler for rendering and displaying task
 */
namespace anisette::core::video
{
    inline SDL_Renderer *renderer = nullptr;
    extern bool refresh_display_info();
} // namespace anisette::core::video


/**
 * @brief Handler for audio output
 */
namespace anisette::core::audio
{
    extern void set_sound_volume(uint8_t volume);
    extern void set_music_volume(uint8_t volume);
    extern bool play_music(const char *path);
    extern void pause_music();
    extern void resume_music();
    extern void stop_music();
} // namespace anisette::core::audio
