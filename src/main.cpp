//
// Created by Yuuki on 19/02/2025.
//
#include "version.h"
#include <core.h>
#include <scene_register.h>
#include <logging.h>
using namespace anisette;

const auto logger = logging::get("main");

int main(const int argc, char *argv[]) {
    logging::init();
    logger->info("Anisette version {}-{}", VERSION, CMAKE_BUILD_TYPE);
    screens::register_scene();
    // pass control to game core
    return core::run();
}

#ifdef WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    return main(0, nullptr);
}
#endif