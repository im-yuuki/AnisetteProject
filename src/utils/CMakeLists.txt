# Logging library
add_library(anisette_log SHARED
        utils/logging/logging.cpp
)
target_include_directories(anisette_log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils/logging)
# link spdlog
target_link_libraries(anisette_log PUBLIC spdlog::spdlog_header_only)

# Discord RPC connection
add_library(anisette_discord SHARED
        utils/discord/discord.cpp
)
target_include_directories(anisette_discord PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils/discord)
target_link_libraries(anisette_discord PRIVATE anisette_log)
# link discord-rpc
find_path(GAME_SDK_INCLUDE discord-game-sdk/discord.h)
target_include_directories(anisette_discord PRIVATE ${GAME_SDK_INCLUDE})
target_link_libraries(anisette_discord PUBLIC ${GAME_SDK})
target_link_libraries(anisette_discord PRIVATE ${CPP_GAME_SDK})

# aliases
add_library(${PROJECT_NAME}::Logging ALIAS anisette_log)
add_library(${PROJECT_NAME}::Discord ALIAS anisette_discord)
