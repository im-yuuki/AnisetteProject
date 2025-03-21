//
// Created by Yuuki on 19/02/2025.
//
#include "version.h"
#include <SDL2/SDL_main.h>
#include <core.h>
#include <logging.h>
#include <csignal>
using namespace anisette;

const auto logger = logging::get("main");

int main(const int argc, char *argv[]) {
    signal(SIGINT, core::handle_interrupt);
    signal(SIGTERM, core::handle_interrupt);
    logging::init();
    logger->info("Anisette version {}-{}", VERSION, CMAKE_BUILD_TYPE);
    // pass control to game core
    return core::run();
}