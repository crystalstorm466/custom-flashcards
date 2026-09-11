cmake_minimum_required(VERSION 3.16)

project(flashcards VERSION 1.0.0 LANGUAGES CXX)

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

add_executable(flashcards ${SOURCES})

target_link_libraries(flashcards PRIVATE
    Qt6::Widgets
)
