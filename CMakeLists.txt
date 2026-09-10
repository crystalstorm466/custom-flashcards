cmake_minimum_required(VERSION 3.16)

project(Flashcards VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's automatic code generators
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    src/main.cpp
    src/Flashcard.h
    src/Flashcard.cpp
    src/Deck.h
    src/Deck.cpp
    src/MainWindow.h
    src/MainWindow.cpp
)

add_executable(Flashcards ${SOURCES})

target_link_libraries(Flashcards PRIVATE
    Qt6::Widgets
)

# RapidJSON dependency resolution:
# 1. Use system RapidJSON if available
find_package(RapidJSON QUIET)
if(TARGET RapidJSON OR TARGET rapidjson)
    target_link_libraries(Flashcards PRIVATE RapidJSON)
# 2. Use git submodule in third_party if present
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidjson/include/rapidjson/rapidjson.h")
    target_include_directories(Flashcards PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidjson/include
    )
# 3. Fallback: Automatically download via FetchContent if someone cloned without --recurse-submodules
else()
    message(STATUS "RapidJSON submodule not found; fetching via FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        RapidJSON
        GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
        GIT_TAG        v1.1.0
        GIT_SHALLOW    TRUE
    )
    FetchContent_GetProperties(RapidJSON)
    if(NOT rapidjson_POPULATED)
        FetchContent_Populate(RapidJSON)
        target_include_directories(Flashcards PRIVATE ${rapidjson_SOURCE_DIR}/include)
    endif()
endif()
