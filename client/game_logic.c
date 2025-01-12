#include "game_logic.h"
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t game_logic_mutex = PTHREAD_MUTEX_INITIALIZER;

void generate_item(GameState* state, int is_obstacle) {
    time_t current_time = time(NULL);

    if (is_obstacle) {
        if (difftime(current_time, state->obstacle_time) >= 10) {
            Obstacle* new_obstacle = (Obstacle*)malloc(sizeof(Obstacle));
            new_obstacle->x = rand() % (state->screen_width - 2) + 1;
            new_obstacle->y = rand() % (state->screen_height - 2) + 1;
            new_obstacle->next = state->obstacles;
            state->obstacles = new_obstacle;
            state->obstacle_time = current_time;
        }
    } else {
        state->food.x = rand() % (state->screen_width - 2) + 1;
        state->food.y = rand() % (state->screen_height - 2) + 1;
    }
}

void init_game(GameState* state, GameOptions* options) {
    srand(time(NULL));

    state->screen_width = options->map_size == Small ? 20 : options->map_size == Medium ? 40 : 60;
    state->screen_height = options->map_size == Small ? 10 : options->map_size == Medium ? 20 : 30;

    pthread_mutex_lock(&game_logic_mutex);
    state->snake_head = (Snake*)malloc(sizeof(Snake));
    state->snake_head->x = state->screen_width / 2;
    state->snake_head->y = state->screen_height / 2;
    state->snake_head->next = NULL;
    state->snake_length = 1;
    state->food_eaten = 0;
    state->obstacles = NULL;
    state->obstacle_time = time(NULL);
    generate_item(state, 0);
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

    Obstacle* current_obstacle = state->obstacles;
    while (current_obstacle) {
        Obstacle* next_obstacle = current_obstacle->next;
        free(current_obstacle);
        current_obstacle = next_obstacle;
    }
    state->obstacles = NULL;

    pthread_mutex_unlock(&game_logic_mutex);
}

void move_snake(GameState* state, int dx, int dy) {
    pthread_mutex_lock(&game_logic_mutex);
    Snake* new_head = (Snake*)malloc(sizeof(Snake));
    new_head->x = (state->snake_head->x + dx + state->screen_width) % state->screen_width;
    new_head->y = (state->snake_head->y + dy + state->screen_height) % state->screen_height;
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

    if (state->snake_head->x <= 0 || state->snake_head->x >= state->screen_width - 1 ||
        state->snake_head->y <= 0 || state->snake_head->y >= state->screen_height - 1) {
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

    if (check_obstacle_collision(state)) {
        pthread_mutex_unlock(&game_logic_mutex);
        return 1;
    }

    pthread_mutex_unlock(&game_logic_mutex);
    return 0;
}

int check_food_collision(GameState* state) {
    pthread_mutex_lock(&game_logic_mutex);
    if (state->snake_head->x == state->food.x && state->snake_head->y == state->food.y) {
        state->food_eaten = 1;
        state->snake_length++;
        generate_item(state, 0);
        pthread_mutex_unlock(&game_logic_mutex);
        return 1;
    }
    pthread_mutex_unlock(&game_logic_mutex);
    return 0;
}

int check_obstacle_collision(GameState* state) {
    Obstacle* current = state->obstacles;
    while (current) {
        if (state->snake_head->x == current->x && state->snake_head->y == current->y) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}
