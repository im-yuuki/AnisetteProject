//
// Created by Yuuki on 16/03/2025.
//
#include "discord.h"
#include <discord-game-sdk/discord.h>
#include <logging.h>

const auto logger = anisette::logging::get("discord");

// alias for discord namespace to avoid conflict with my own namespace
namespace discord_sdk = discord;

discord_sdk::Activity init_presence() {
    discord_sdk::Activity presence;
    // default values
    presence.SetType(discord_sdk::ActivityType::Playing);
    presence.GetAssets().SetLargeImage("icon");
    presence.GetAssets().SetLargeText("Anisette");
    return presence;
}

namespace anisette::utils::discord
{
    static discord_sdk::Core *core = nullptr;
    static discord_sdk::Activity presence = init_presence();

    void start() {
        if (core) return;
        logger->debug("Initializing Discord RPC connection");
        if (const auto result = discord_sdk::Core::Create(DISCORD_APPLICATION_ID, DiscordCreateFlags_NoRequireDiscord, &core); result != discord_sdk::Result::Ok) {
            logger->error("Initialize Discord RPC failed: core return error code {}", static_cast<int>(result));
            delete core;
            core = nullptr;
        }
    }

    void shutdown() {
        if (!core) return;
        logger->debug("Shutting down Discord RPC connection");
        delete core;
        core = nullptr;
    }

    void poll() {
        if (!core) return;
        core->ActivityManager().UpdateActivity(presence, [](discord_sdk::Result result) {
            if (result == discord_sdk::Result::Ok) return;
            logger->error("Failed to update Discord presence: code {}", static_cast<int>(result));
        });
        core->RunCallbacks();
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