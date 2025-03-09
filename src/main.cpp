//
// Created by Yuuki on 19/02/2025.
//
#include <logging.h>
#include "version.h"
using namespace anisette;

int main(int argc, char *argv[]) {
    logging::init();
    logging::get("default")->info("Starting Anisette version {}", VERSION);
}