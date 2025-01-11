#include "game_logic.h"
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 20

pthread_mutex_t game_logic_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_game(GameState* state) {
    srand(time(NULL));

    pthread_mutex_lock(&game_logic_mutex);
    state->snake_head = (Snake*)malloc(sizeof(Snake));
    state->snake_head->x = SCREEN_WIDTH / 2;
    state->snake_head->y = SCREEN_HEIGHT / 2;
    state->snake_head->next = NULL;
    state->snake_length = 1;
    state->food_eaten = 0;
    generate_food(state);
    pthread_mutex_unlock(&game_logic_mutex);
}

void cleanup_game(GameState* state) {
    pthread_mutex_lock(&game_logic_mutex);
    Snake* current = state->snake_head;
    while (current) {
        Snake* next = current->next;
        free(current);
        current = next;
    }
    state->snake_head = NULL;
    pthread_mutex_unlock(&game_logic_mutex);
}

void move_snake(GameState* state, int dx, int dy) {
    pthread_mutex_lock(&game_logic_mutex);
    Snake* new_head = (Snake*)malloc(sizeof(Snake));
    new_head->x = (state->snake_head->x + dx + SCREEN_WIDTH) % SCREEN_WIDTH;
    new_head->y = (state->snake_head->y + dy + SCREEN_HEIGHT) % SCREEN_HEIGHT;
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
    pthread_mutex_unlock(&game_logic_mutex);
}

int check_collision(GameState* state) {
    pthread_mutex_lock(&game_logic_mutex);

    if (state->snake_head->x <= 0 || state->snake_head->x >= SCREEN_WIDTH - 1 ||
        state->snake_head->y <= 0 || state->snake_head->y >= SCREEN_HEIGHT - 1) {
        pthread_mutex_unlock(&game_logic_mutex);
        return 1;
        }

    Snake* current = state->snake_head->next;
    while (current) {
        if (current->x == state->snake_head->x && current->y == state->snake_head->y) {
            pthread_mutex_unlock(&game_logic_mutex);
            return 1;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&game_logic_mutex);
    return 0;
}

void generate_food(GameState* state) {
    state->food.x = rand() % (SCREEN_WIDTH - 2) + 1;
    state->food.y = rand() % (SCREEN_HEIGHT - 2) + 1;
}

int check_food_collision(GameState* state) {
    pthread_mutex_lock(&game_logic_mutex);
    if (state->snake_head->x == state->food.x && state->snake_head->y == state->food.y) {
        state->food_eaten = 1;
        state->snake_length++;
        generate_food(state);
        pthread_mutex_unlock(&game_logic_mutex);
        return 1;
    }
    pthread_mutex_unlock(&game_logic_mutex);
    return 0;
}