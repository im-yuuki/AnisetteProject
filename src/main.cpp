//
// Created by Yuuki on 19/02/2025.
//
#include "version.h"
#include <core.h>
#include <logging.h>
#include <csignal>
using namespace anisette;

const auto logger = logging::get("main");

int main(const int argc, char *argv[]) {
    signal(SIGINT, core::event::handle_interrupt);
    signal(SIGTERM, core::event::handle_interrupt);
    logging::init();
    logger->info("Anisette version {}-{}", VERSION, CMAKE_BUILD_TYPE);
    // pass control to game core
    return core::run();
}

#ifdef WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    // handle interrupt signals
    signal(SIGINT, core::event::handle_interrupt);
    signal(SIGTERM, core::event::handle_interrupt);
    logging::init();
    logger->info("Anisette version {}-{}", VERSION, CMAKE_BUILD_TYPE);
    // pass control to game core
    return core::run();
}
#endif