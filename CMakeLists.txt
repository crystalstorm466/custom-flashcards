cmake_minimum_required(VERSION 3.16)

project(flashcards VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's automatic code generators
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Test Pdf)
enable_testing(true)

# Core source files excluding main.cpp so they can be shared with tests
set(CORE_SOURCES
    src/Flashcard.h
    src/Flashcard.cpp
    src/Deck.h
    src/Deck.cpp
    src/vimBinds.h
    src/vimBinds.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    resources/resources.qrc
)

# Full application sources including main.cpp
set(SOURCES
    src/main.cpp
    ${CORE_SOURCES}
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(QT_NO_DEBUG_OUTPUT) #compiling for release supresses all qDebug() statements
endif()

qt_add_executable(mytest tests/unitTests.cpp)
target_link_libraries(mytest PRIVATE Qt6::Widgets Qt6::Test)
add_test(NAME mytest COMMAND mytest)

qt_add_executable(deckTest tests/test_jsonImports.cpp ${CORE_SOURCES})
target_link_libraries(deckTest PRIVATE Qt6::Widgets Qt6::Test Qt6::Pdf)
add_test(NAME deckTest COMMAND deckTest)

add_executable(flashcards ${SOURCES})
target_link_libraries(flashcards PRIVATE Qt6::Widgets)
