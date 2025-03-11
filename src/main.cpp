//
// Created by Yuuki on 19/02/2025.
//
#include "version.h"
#include <core.h>
#include <logging.h>
using namespace anisette;

int main(const int argc, char *argv[]) {
    logging::init();
    logging::get("default")->info("Anisette version {}", VERSION);
    // pass control to game core
    return core::run(argc, argv);
}