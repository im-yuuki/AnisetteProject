# Utils library
add_library(anisette_utils SHARED
        utils/logging.cpp
        utils/discord.cpp
)
target_include_directories(anisette_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils)
# link spdlog
target_link_libraries(anisette_utils PUBLIC spdlog::spdlog_header_only)
# link discord-rpc
find_path(GAME_SDK_INCLUDE discord-game-sdk/discord.h)
target_link_libraries(anisette_utils PUBLIC ${GAME_SDK} ${CPP_GAME_SDK})
