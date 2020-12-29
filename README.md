# Battleship Game
## About
This is a popular strategy board game for two players. Each player places few ships on the grid and the locations of ships is unknown to the other player. They then take turns at guessing and shooting opponent's ships. First player to shoot down all of other player's ships - wins.

## How to run
This project is created for Linux, written in C++ with SFML library.
First you have to compile the project. Unfortunately, to do that, you need to install a lot of SFML's dependencies first, which are listed here: https://www.sfml-dev.org/tutorials/2.5/compile-with-cmake.php

Then run these commands to generate and compile the project:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```
You can obviously use your favorite generator (e.g. ninja) instead of make.

Binaries are stored in `bin` directory in project directory. To open the game simply run:
```
$ bin/Battleship
```