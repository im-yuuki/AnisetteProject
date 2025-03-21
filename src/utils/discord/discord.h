//
// Created by Yuuki on 16/03/2025.
//
#pragma once

// you can define your own application ID in the build system
#ifndef DISCORD_APPLICATION_ID
#define DISCORD_APPLICATION_ID 1350787768351068180
#endif

/**
 * Anisette Discord RPC connection module
 *
 * @author Yuuki
 * @date 16/03/2025
 * @version 1.0.0
 */
namespace anisette::utils::discord
{
    /**
     * @brief Start the Discord RPC connection
     */
    void start();

    /**
     * @brief Shutdown the Discord RPC connection
     */
    void shutdown();

    /**
     * @brief Update the Discord RPC presence
     *
     * This function needs to be called periodically to commit the current presence mode to Discord client.
     */
    void poll();

    /**
     * @brief Set the Discord presence status to idle mode
     */
    void set_idle();

    /**
     * @brief Set the Discord presence status to in main menu mode
     */
    void set_in_main_menu();

    /**
     * @brief Set the Discord presence status to browsing library mode
     */
    void set_browsing_libary();

    /**
     * @brief Set the Discord presence status to playing a song
     *
     * @param song Song name
     * @param difficulty Difficulty level
     */
    void set_playing_song(const char *song, const char* difficulty);

    /**
     * @brief Set the Discord presence status to spectating a song
     *
     * @param song Song name
     * @param difficulty Difficulty level
     */
    void set_spectate_song(const char *song, const char* difficulty);
}