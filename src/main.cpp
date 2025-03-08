//
// Created by Yuuki on 19/02/2025.
//
#include <logging.h>
using namespace game;

int main(int argc, char *argv[]) {
    const auto logger = logging::Logger::get(__FILE__);
    logger->info("Hello, World!");
    logger->debug("This is a debug message");
    return 0;
}