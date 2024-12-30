#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <SDL2/SDL.h>

typedef struct Snake {
    int x, y;
    struct Snake* next;
} Snake;

typedef struct GameState {
    Snake* snake_head;
    int snake_length;
    int food_eaten;
    SDL_Rect food;
} GameState;

void init_game(GameState* state);
void move_snake(GameState* state, int dx, int dy);
int check_collision(GameState* state);
void generate_food(GameState* state);
int check_food_collision(GameState* state);
void draw_snake(SDL_Renderer* renderer, GameState* state);
void draw_food(SDL_Renderer* renderer, GameState* state);
void cleanup_game(GameState* state);

#endif
