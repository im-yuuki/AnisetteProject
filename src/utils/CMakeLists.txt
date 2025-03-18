# Logging library
add_library(libani_log SHARED
        utils/logging/logging.cpp
)
target_include_directories(libani_log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils/logging)
# link spdlog
target_link_libraries(libani_log PUBLIC spdlog::spdlog_header_only)

# Discord RPC connection
add_library(libani_discord SHARED
        utils/discord/discord.cpp
)
target_include_directories(libani_discord PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils/discord)
target_link_libraries(libani_discord PRIVATE libani_log)
# link discord-rpc
target_link_libraries(libani_discord PUBLIC discord-rpc)

# aliases
add_library(${PROJECT_NAME}::Logging ALIAS libani_log)
add_library(${PROJECT_NAME}::Discord ALIAS libani_discord)

