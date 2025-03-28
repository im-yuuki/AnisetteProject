//
// Created by Yuuki on 16/03/2025.
//
#include "discord.h"
#include <../../build/x64-windows-dbg/vcpkg_installed/x64-windows/include/discord-game-sdk/discord.h>
#include <logging.h>

const auto logger = anisette::logging::get("discord");

// alias for discord namespace to avoid conflict with my own namespace
namespace discord_sdk = discord;

namespace anisette::utils::discord
{
    static std::atomic_bool is_ready = true;
    static discord_sdk::Core *core = nullptr;
    static discord_sdk::Activity presence {};

    void start() {
        // default values
        presence.SetType(discord_sdk::ActivityType::Playing);
        presence.GetAssets().SetLargeImage("icon");
        presence.GetAssets().SetLargeText("Anisette");
        presence.SetState("In the main menu");
        presence.SetDetails("");
        // initialize Discord RPC connection
        logger->debug("Initializing Discord RPC connection");
        if (const auto result = discord_sdk::Core::Create(DISCORD_APPLICATION_ID, DiscordCreateFlags_NoRequireDiscord, &core); result != discord_sdk::Result::Ok) {
            logger->error("Initialize Discord RPC failed: core return error code {}", static_cast<int>(result));
            return;
        }
        is_ready = true;
        poll(); // first update, later updates will be handled by the main loop
    }

    void shutdown() {
        logger->debug("Shutting down Discord RPC connection");
        is_ready = false;
        delete core;
    }

    void poll() {
        if (is_ready && core) {
            core->ActivityManager().UpdateActivity(presence, [](discord_sdk::Result result) {
                if (result == discord_sdk::Result::Ok) return;
                if (!is_ready) return;
                logger->error("Failed to update Discord presence: code {}", static_cast<int>(result));
            });
            core->RunCallbacks();
        }
    }

    void set_idle() {
        logger->debug("Set Discord presence to idle mode");
        presence.SetState("Idle");
        presence.SetDetails("");
        presence.GetAssets().SetSmallImage("idle");
        presence.GetAssets().SetSmallText("Idle");
    }

    void set_in_main_menu() {
        logger->debug("Set Discord presence to main menu mode");
        presence.SetState("In the main menu");
        presence.SetDetails(nullptr);
        presence.GetAssets().SetSmallImage("home");
        presence.GetAssets().SetSmallText("Home");
    }

    void set_browsing_library() {
        logger->debug("Set Discord presence to browsing library mode");
        presence.SetState("Browsing songs");
        presence.SetDetails("");
        presence.GetAssets().SetSmallImage("browse");
        presence.GetAssets().SetSmallText("Browsing");
    }

    void set_playing_song(const char *song, const char* difficulty) {
        logger->debug("Set Discord presence to playing song");
        presence.SetState(song);
        presence.SetDetails(difficulty);
        presence.GetAssets().SetSmallImage("play");
        presence.GetAssets().SetSmallText("Playing");
    }

    void set_spectate_song(const char *song, const char *difficulty) {
        logger->debug("Set Discord presence to spectating songs");
        presence.SetState(song);
        presence.SetDetails(difficulty);
        presence.GetAssets().SetSmallImage("spectate");
        presence.GetAssets().SetSmallText("Spectating");
    }
}