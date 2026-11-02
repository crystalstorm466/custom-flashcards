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
- Importing a million card deck and verifying that the cards are imported

Run:

```
./build/deckTest

```


Run the flashcards binary inside build/ to run the program. 

