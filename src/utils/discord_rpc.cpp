//
// Created by Yuuki on 16/03/2025.
//
#include <discord_rpc.h>
#include <version.h>
#include <logging.h>
#include <discord-game-sdk/core.h>
using namespace discord;

constexpr uint64_t DISCORD_CORE_FLAGS = DiscordCreateFlags_Default;
const auto logger = anisette::logging::get("discord");
static Core* core;

namespace anisette::utils::discord {
    void init() {
        const auto result = Core::Create(DISCORD_CLIENT_ID, DISCORD_CORE_FLAGS, &core);
        if (result != Result::Ok) {
            logger->error("Failed to initialize Discord SDK core: {}", result);
            return;
        }
    }
}