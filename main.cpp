#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

// Custom to_string function
template <typename T>
std::string to_string(T value) {
    std::ostringstream os;
    os << value;
    return os.str();
}

const int TILE_SIZE = 32;
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 10;
const int MINES_COUNT = 10;

enum TileState { HIDDEN, REVEALED, FLAGGED };

struct Tile {
    bool hasMine;
    int adjacentMines;
    TileState state;
};

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;
std::vector<std::vector<Tile>> grid(GRID_HEIGHT, std::vector<Tile>(GRID_WIDTH));
bool gameRunning = true;
bool gameWon = false;

void RenderText(const std::string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (surface == nullptr) {
        std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void InitGrid() {
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            grid[i][j] = { false, 0, HIDDEN };
        }
    }

    int placedMines = 0;
    while (placedMines < MINES_COUNT) {
        int x = rand() % GRID_WIDTH;
        int y = rand() % GRID_HEIGHT;
        if (!grid[y][x].hasMine) {
            grid[y][x].hasMine = true;
            placedMines++;
        }
    }

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (grid[y][x].hasMine) continue;
            int count = 0;
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    int ny = y + i;
                    int nx = x + j;
                    if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT && grid[ny][nx].hasMine) {
                        count++;
                    }
                }
            }
            grid[y][x].adjacentMines = count;
        }
    }
}

void RevealTile(int x, int y) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT || grid[y][x].state == REVEALED) {
        return;
    }
    grid[y][x].state = REVEALED;
    if (grid[y][x].hasMine) {
        gameRunning = false;
        gameWon = false;
        return;
    }
    if (grid[y][x].adjacentMines == 0 && !grid[y][x].hasMine) {
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                RevealTile(x + j, y + i);
            }
        }
    }

    // Check if the player has won
    bool won = true;
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (!grid[y][x].hasMine && grid[y][x].state != REVEALED) {
                won = false;
                break;
            }
        }
        if (!won) break;
    }
    if (won) {
        gameRunning = false;
        gameWon = true;
    }
}

void RenderGrid() {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            SDL_Rect rect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            if (grid[y][x].state == REVEALED) {
                if (grid[y][x].hasMine) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red for mines
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);  // Darker gray for revealed tiles
                }
            }
            else {
                SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);  // Light gray for hidden tiles
            }
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);

            if (grid[y][x].state == REVEALED && !grid[y][x].hasMine) {
                std::string text = grid[y][x].adjacentMines > 0 ? to_string(grid[y][x].adjacentMines) : "";
                if (!text.empty()) {
                    RenderText(text, x * TILE_SIZE + 10, y * TILE_SIZE + 5, { 0, 0, 0, 255 });
                }
            }
        }
    }

    if (!gameRunning) {
        std::string message = gameWon ? "You Win!" : "Game Over!";
        RenderText(message, GRID_WIDTH * TILE_SIZE / 2 - 50, GRID_HEIGHT * TILE_SIZE / 2 - 40, { 255, 255, 255, 255 });
        RenderText("Press Enter to Restart", GRID_WIDTH * TILE_SIZE / 2 - 90, GRID_HEIGHT * TILE_SIZE / 2, { 255, 255, 255, 255 });
        RenderText("Press Escape to Quit", GRID_WIDTH * TILE_SIZE / 2 - 90, GRID_HEIGHT * TILE_SIZE / 2 + 30, { 255, 255, 255, 255 });
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() < 0) {
        std::cerr << "Could not initialize SDL_ttf: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    window = SDL_CreateWindow("Minesweeper",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        GRID_WIDTH * TILE_SIZE, GRID_HEIGHT * TILE_SIZE,
        SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Could not create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    font = TTF_OpenFont("font.ttf", 24);
    if (!font) {
        std::cerr << "Could not load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    InitGrid();

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && gameRunning) {
                int x = event.button.x / TILE_SIZE;
                int y = event.button.y / TILE_SIZE;
                if (event.button.button == SDL_BUTTON_LEFT) {
                    RevealTile(x, y);
                }
            }
            if (!gameRunning && event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN) {
                    InitGrid();
                    gameRunning = true;
                    gameWon = false;
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }

        RenderGrid();
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
