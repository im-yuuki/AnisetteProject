//
// Created by Yuuki on 19/02/2025.
//
#include "version.h"
#include <core.h>
#include <logging.h>
using namespace anisette;

int main(const int argc, char *argv[]) {
    if (strcmp(CMAKE_BUILD_TYPE, "Release") != 0) init(logging::DEBUG);
    else init(logging::INFO);
    logging::get("default")->info("Anisette version {}-{}", VERSION, CMAKE_BUILD_TYPE);
    // pass control to game core
    return core::run();
}