# Utils library
add_library(anisette_utils SHARED
        utils/logging.cpp
        utils/discord.cpp
)
# link spdlog
target_link_libraries(anisette_utils PUBLIC spdlog::spdlog_header_only)
# link discord-rpc
find_path(GAME_SDK_INCLUDE discord-game-sdk/discord.h)
target_link_libraries(anisette_utils PUBLIC ${GAME_SDK} ${CPP_GAME_SDK})
# aliases
add_library(${PROJECT_NAME}::Utils ALIAS anisette_utils)
