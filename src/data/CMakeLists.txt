add_library(anisette_data SHARED
        data/loader.cpp
        data/beatmap.cpp
)

target_link_libraries(anisette_data PUBLIC RapidJSON rapidjson)
