# Flashcards 

This program is written in C++ and for QT. 

Dependencies:

```
GCC 
QT 6.11

```

To Compile:

```
    git clone https://git.cardinalsystem.net/david/custom-flashcards.git
    cd custom-flashcards/
    cmake -B build -DCMAKE_BUILD_TYPE=Release -S . (to suppress warnings in terminal)
    cmake -B build -DCMAKE_BUILD_TYPE=Debug -S . (to show all warnings)

    cmake --build build
```


New Feature: Introducing unit tests

Current tests available:
- Test initial state
- Test exporting empty decks
- test importing big json files (10k cards) and manipulating them
- removing and reimporting big json files
- exporting jsons to pdf and json
- exporting single card decks
- testing corrupted card decks from corrupted jsons

Run:

```
./build/deckTest

```


Run the flashcards binary inside build/ to run the program. 

