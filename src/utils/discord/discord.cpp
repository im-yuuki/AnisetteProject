//
// Created by Yuuki on 16/03/2025.
//
#include "discord.h"
#include <version.h>
#include <logging.h>
#include <discord_rpc.h>

const auto logger = anisette::logging::get("discord");
static bool is_ready = true;

static DiscordRichPresence presence {
    .state = "In the main menu",
    .details = nullptr,
    .startTimestamp = 0,
    .endTimestamp = 0,
    .largeImageKey = "icon",
    .largeImageText = "Anisette",
    .smallImageKey = "home",
    .smallImageText = "Home",
};

namespace anisette::utils::discord {
    void init() {
        logger->info("Initializing Discord RPC connection");
        Discord_Initialize(DISCORD_APPLICATION_ID, nullptr, 1, nullptr);
        is_ready = true;
    }

    void cleanup() {
        logger->info("Shutting down Discord RPC connection");
        is_ready = false;
        Discord_Shutdown();
    }

    void update() {
        if (is_ready) Discord_UpdatePresence(&presence);
    }

    void set_idle() {
        logger->debug("Set Discord presence to idle mode");
        presence.state = "Idle";
        presence.details = nullptr;
        presence.smallImageKey = "idle";
        presence.smallImageText = "Idle";
    }


    void set_in_main_menu() {
        logger->debug("Set Discord presence to main menu mode");
        presence.state = "In the main menu";
        presence.details = nullptr;
        presence.smallImageKey = "home";
        presence.smallImageText = "Home";
    }

    void set_browsing_libary() {
        logger->debug("Set Discord presence to browsing library mode");
        presence.state = "Browsing songs";
        presence.details = nullptr;
        presence.smallImageKey = "browse";
        presence.smallImageText = "Browsing";
    }

    void set_playing_song(const char *song, const char* difficulty) {
        logger->debug("Set Discord presence to playing song");
        presence.state = song;
        presence.details = difficulty;
        presence.smallImageKey = "play";
        presence.smallImageText = "Playing";
    }

    void set_spectate_song(const char *song, const char *difficulty) {
        logger->debug("Set Discord presence to spectating songs");
        presence.state = song;
        presence.details = difficulty;
        presence.smallImageKey = "spectate";
        presence.smallImageText = "Spectating";
    }

}