# cmake/nlohmann_json.cmake

include(FetchContent)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2 # Specify the version you want
)

FetchContent_GetProperties(nlohmann_json)
if (NOT nlohmann_json_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(nlohmann_json)
    add_subdirectory(${nlohmann_json_SOURCE_DIR} ${nlohmann_json_BINARY_DIR})
endif()