add_library(anisette_data SHARED
    data/dynamic_loader.cpp
)

target_link_libraries(anisette_data PUBLIC RapidJSON rapidjson)
