#include "game_logic.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800 //neskor mozno spravit upravitelne?
#define SCREEN_HEIGHT 600
#define SNAKE_SIZE 20

void init_game(GameState* state) {
    srand(time(NULL));

    state->snake_head = (Snake*)malloc(sizeof(Snake));
    state->snake_head->x = SCREEN_WIDTH / 2;
    state->snake_head->y = SCREEN_HEIGHT / 2;
    state->snake_head->next = NULL;
    state->snake_length = 1;
    state->food_eaten = 0;

    generate_food(state);
}

void move_snake(GameState* state, int dx, int dy) {
    Snake* new_head = (Snake*)malloc(sizeof(Snake));
    new_head->x = state->snake_head->x + dx * SNAKE_SIZE;
    new_head->y = state->snake_head->y + dy * SNAKE_SIZE;
    new_head->next = state->snake_head;
    state->snake_head = new_head;

    if (state->food_eaten) {
        state->food_eaten = 0;
    } else {
        Snake* current = state->snake_head;
        while (current->next && current->next->next) {
            current = current->next;
        }
        free(current->next);
        current->next = NULL;
    }
}

int check_collision(GameState* state) {

    if (state->snake_head->x < 0 || state->snake_head->x >= SCREEN_WIDTH ||
        state->snake_head->y < 0 || state->snake_head->y >= SCREEN_HEIGHT) {
        return 1;
    }

    Snake* current = state->snake_head->next;
    while (current) {
        if (current->x == state->snake_head->x && current->y == state->snake_head->y) {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

void generate_food(GameState* state) {
    state->food.x = (rand() % (SCREEN_WIDTH / SNAKE_SIZE)) * SNAKE_SIZE;
    state->food.y = (rand() % (SCREEN_HEIGHT / SNAKE_SIZE)) * SNAKE_SIZE;
    state->food.w = SNAKE_SIZE;
    state->food.h = SNAKE_SIZE;
}

int check_food_collision(GameState* state) {
    if (state->snake_head->x == state->food.x && state->snake_head->y == state->food.y) {
        state->food_eaten = 1;
        state->snake_length++;
        generate_food(state);
        return 1;
    }
    return 0;
}

void draw_snake(SDL_Renderer* renderer, GameState* state) {
    Snake* current = state->snake_head;
    while (current) {
        SDL_Rect rect = {current->x, current->y, SNAKE_SIZE, SNAKE_SIZE};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
        current = current->next;
    }
}

void draw_food(SDL_Renderer* renderer, GameState* state) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &state->food);
}

void cleanup_game(GameState* state) {
    Snake* current = state->snake_head;
    while (current) {
        Snake* next = current->next;
        free(current);
        current = next;
    }
}
