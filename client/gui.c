#include "gui.h"
#include "game_logic.h"
#include <SDL2/SDL.h>

int init_gui(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize!");
        return 0;
    }

    *window = SDL_CreateWindow(
        "Snake Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

    return 1;
}


void run_game(SDL_Renderer* renderer) {
    GameState state;
    int dx = 1, dy = 0;
    int running = 1;
    SDL_Event event;

    init_game(&state);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:    if (dy == 0) { dx = 0; dy = -1; } break;
                    case SDLK_DOWN:  if (dy == 0) { dx = 0; dy = 1; } break;
                    case SDLK_LEFT:  if (dx == 0) { dx = -1; dy = 0; } break;
                    case SDLK_RIGHT: if (dx == 0) { dx = 1; dy = 0; } break;
                }
            }
        }

        move_snake(&state, dx, dy);

        if (check_collision(&state)) {
            printf("Game Over\n");
            running = 0;
        }

        if (check_food_collision(&state)) {
            printf("Score: %d\n", state.snake_length);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        draw_snake(renderer, &state);
        draw_food(renderer, &state);

        SDL_RenderPresent(renderer);
        SDL_Delay(100); //meni sa rychlost - neskor mozno spravit difficulty podla menenia rychlosti
    }

    cleanup_game(&state);
}

void cleanup_gui(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}