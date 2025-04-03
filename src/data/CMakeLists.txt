add_library(anisette_data STATIC
        data/loader.cpp
        data/beatmap.cpp
)
target_include_directories(anisette_data PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/data)
target_link_libraries(anisette_data PUBLIC RapidJSON rapidjson)
target_link_libraries(anisette_data PUBLIC anisette_utils)
