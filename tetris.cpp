#include <SDL.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int FIELD_WIDTH = 12;
const int FIELD_HEIGHT = 18;
const int BLOCK_SIZE = 24;

unsigned char field[FIELD_HEIGHT * FIELD_WIDTH];
std::string tetromino[7];

int Rotate(int px, int py, int r) {
    switch (r % 4) {
        case 0: return py * 4 + px;
        case 1: return 12 + py - (px * 4);
        case 2: return 15 - (py * 4) - px;
        case 3: return 3 - py + (px * 4);
    }
    return 0;
}

bool DoesPieceFit(int tetrominoIndex, int rotation, int posX, int posY) {
    for (int px = 0; px < 4; px++) {
        for (int py = 0; py < 4; py++) {
            int pi = Rotate(px, py, rotation);
            int fi = (posY + py) * FIELD_WIDTH + (posX + px);
            if (posX + px >= 0 && posX + px < FIELD_WIDTH && posY + py >= 0 && posY + py < FIELD_HEIGHT) {
                if (tetromino[tetrominoIndex][pi] == 'X' && field[fi] != 0)
                    return false;
            }
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(0)));

    tetromino[0] = "..X...X...X...X.";
    tetromino[1] = "..X..XX...X.....";
    tetromino[2] = ".....XX..XX.....";
    tetromino[3] = "..X..XX..X......";
    tetromino[4] = ".X...XX...X.....";
    tetromino[5] = ".X...X...XX.....";
    tetromino[6] = "..X...X..XX.....";

    for (int x = 0; x < FIELD_WIDTH; x++) {
        for (int y = 0; y < FIELD_HEIGHT; y++) {
            field[y * FIELD_WIDTH + x] = (x == 0 || x == FIELD_WIDTH - 1 || y == FIELD_HEIGHT - 1) ? 9 : 0;
        }
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        std::cout << "Ошибка инициализации SDL: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("Tetris SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Ошибка создания окна: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool gameOver = false;
    int currentPiece = rand() % 7;
    int currentRotation = 0;
    int currentX = FIELD_WIDTH / 2 - 2;
    int currentY = 0;
    int speed = 20;
    int speedCounter = 0;
    bool forceDown = false;
    int score = 0;
    std::vector<int> lines;

    Uint32 lastTick = SDL_GetTicks();

    while (!gameOver) {
        Uint32 currentTick = SDL_GetTicks();
        Uint32 deltaTick = currentTick - lastTick;
        lastTick = currentTick;
        speedCounter += deltaTick;
        forceDown = (speedCounter >= speed * 50);

        if (forceDown) {
            speedCounter = 0;
            if (DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1))
                currentY++;
            else {
                for (int px = 0; px < 4; px++) {
                    for (int py = 0; py < 4; py++) {
                        if (tetromino[currentPiece][Rotate(px, py, currentRotation)] == 'X')
                            field[(currentY + py) * FIELD_WIDTH + (currentX + px)] = currentPiece + 1;
                    }
                }
                for (int py = 0; py < 4; py++) {
                    if (currentY + py < FIELD_HEIGHT - 1) {
                        bool line = true;
                        for (int x = 1; x < FIELD_WIDTH - 1; x++)
                            line &= (field[(currentY + py) * FIELD_WIDTH + x]) != 0;
                        if (line) {
                            for (int x = 1; x < FIELD_WIDTH - 1; x++)
                                field[(currentY + py) * FIELD_WIDTH + x] = 8;
                            lines.push_back(currentY + py);
                        }
                    }
                }
                score += 25;
                if (!lines.empty())
                    score += (1 << lines.size()) * 100;
                currentPiece = rand() % 7;
                currentRotation = 0;
                currentX = FIELD_WIDTH / 2 - 2;
                currentY = 0;
                if (!DoesPieceFit(currentPiece, currentRotation, currentX, currentY))
                    gameOver = true;
            }
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                gameOver = true;
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (DoesPieceFit(currentPiece, currentRotation, currentX - 1, currentY))
                            currentX--;
                        break;
                    case SDLK_RIGHT:
                        if (DoesPieceFit(currentPiece, currentRotation, currentX + 1, currentY))
                            currentX++;
                        break;
                    case SDLK_DOWN:
                        if (DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1))
                            currentY++;
                        break;
                    case SDLK_UP:
                        if (DoesPieceFit(currentPiece, currentRotation + 1, currentX, currentY))
                            currentRotation++;
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int x = 0; x < FIELD_WIDTH; x++) {
            for (int y = 0; y < FIELD_HEIGHT; y++) {
                if (field[y * FIELD_WIDTH + x] != 0) {
                    SDL_Rect rect = { x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE - 1, BLOCK_SIZE - 1 };
                    int val = field[y * FIELD_WIDTH + x];
                    if (val == 9)
                        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
                    else if (val == 8)
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    else {
                        switch(val) {
                            case 1: SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); break;
                            case 2: SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); break;
                            case 3: SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); break;
                            case 4: SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); break;
                            case 5: SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); break;
                            case 6: SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); break;
                            case 7: SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); break;
                        }
                    }
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        for (int px = 0; px < 4; px++) {
            for (int py = 0; py < 4; py++) {
                if (tetromino[currentPiece][Rotate(px, py, currentRotation)] == 'X') {
                    SDL_Rect rect = { (currentX + px) * BLOCK_SIZE, (currentY + py) * BLOCK_SIZE, BLOCK_SIZE - 1, BLOCK_SIZE - 1 };
                    switch(currentPiece + 1) {
                        case 1: SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); break;
                        case 2: SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); break;
                        case 3: SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); break;
                        case 4: SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); break;
                        case 5: SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); break;
                        case 6: SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); break;
                        case 7: SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); break;
                    }
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        SDL_RenderPresent(renderer);

        if (!lines.empty()) {
            SDL_Delay(400);
            for (auto &line : lines) {
                for (int x = 1; x < FIELD_WIDTH - 1; x++) {
                    for (int y = line; y > 0; y--) {
                        field[y * FIELD_WIDTH + x] = field[(y - 1) * FIELD_WIDTH + x];
                    }
                    field[x] = 0;
                }
            }
            lines.clear();
        }
    }

    std::cout << "Game Over! Score: " << score << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
