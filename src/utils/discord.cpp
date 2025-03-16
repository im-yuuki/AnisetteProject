//
// Created by Yuuki on 16/03/2025.
//
#include <discord.h>
#include <version.h>
#include <logging.h>
#include <discord_rpc.h>
#include <thread>

const auto logger = anisette::logging::get("discord");
static std::mutex mutex;
static std::atomic_bool is_ready {false}, stop_requested {false};

static void handle_ready(const DiscordUser *request) {
    logger->info("Connected to Discord RPC as {}", request->username);
    is_ready = true;
}

static void handle_disconnected(int code, const char *message) {
    logger->warn("Disconnected from Discord RPC: {} ({})", message, code);
    is_ready = false;
}

static void handle_error(int code, const char *message) {
    logger->error("Discord RPC error: {} ({})", message, code);
}

static DiscordEventHandlers event_handlers {handle_ready, handle_disconnected, handle_error, nullptr, nullptr};
static DiscordRichPresence presence {};

namespace anisette::utils::discord {
    void start_thread() {
        std::thread thread([&]() {
            logger->info("Starting RPC connection");
            Discord_Initialize(DISCORD_APPLICATION_ID, &event_handlers, 1, nullptr);
            while (!stop_requested) {
               if (is_ready) {
                   std::lock_guard lock(mutex);
                   Discord_UpdatePresence(&presence);
               }
               std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            logger->info("Shutting down RPC connection");
            Discord_Shutdown();
        });
    }
}