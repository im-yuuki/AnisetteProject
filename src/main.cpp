//
// Created by Yuuki on 19/02/2025.
//
#include <core.h>
#include <logging.h>
#include "version.h"
using namespace anisette;

int main(const int argc, char *argv[]) {
    logging::init();
    logging::get("default")->info("Starting Anisette version {}", VERSION);
    return core::start(argc, argv);
}