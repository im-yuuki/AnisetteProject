//
// Created by Yuuki on 19/02/2025.
//
#include "version.h"
#include "core/core.h"
#include "utils/logging.h"
#include "screens/register.h"
#include <iostream>
using namespace anisette;

const auto logger = logging::get("main");

int main() {
    std::cout << R"(
Anisette Copyright (C) 2025 Yuuki (https://github.com/im-yuuki)
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions
)" << std::endl;
    //
    logging::init();
    logger->info("Starting game version {}-{}", VERSION, CMAKE_BUILD_TYPE);
    // register screens
    screens::load();
    // pass control to game core
    return core::run();
}

#ifdef WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    return main();
}
#endif