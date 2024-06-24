# Minesweeper Game

This is a simple implementation of the classic Minesweeper game using SDL and C++.

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Controls](#controls)
- [Features](#features)
- [Contributing](#contributing)
- [License](#license)

## Installation

### Prerequisites

- SDL2
- SDL2_ttf
- C++ compiler (e.g., g++)

### Steps

1. Clone the repository:

    ```sh
    git clone https://github.com/your-username/minesweeper-game.git
    cd minesweeper-game
    ```

2. Install dependencies:

    For Linux (Ubuntu/Debian):

    ```sh
    sudo apt-get install libsdl2-dev libsdl2-ttf-dev
    ```

    For MacOS (using Homebrew):

    ```sh
    brew install sdl2 sdl2_ttf
    ```

    For Windows:

    Download SDL2 and SDL2_ttf development libraries from the SDL website and add them to your project.

3. Compile the code:

    ```sh
    g++ -o minesweeper main.cpp -lSDL2 -lSDL2_ttf
    ```

## Usage

Run the executable:

```sh
./minesweeper
