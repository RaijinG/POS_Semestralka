#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

typedef struct Snake {
    int x, y;
    struct Snake* next;
} Snake;

typedef struct GameState {
    Snake* snake_head;
    int snake_length;
    int food_eaten;
    struct {
        int x, y;
    } food;
    int screen_width;
    int screen_height;
} GameState;

typedef enum {
    Easy = 1,
    Normal,
    Hard
} Difficulty;

typedef enum {
    Small = 1,
    Medium,
    Large
} MapSize;

typedef enum {
    Classic = 1,
    Challenge
} GameMode;

typedef struct {
    Difficulty difficulty;
    MapSize map_size;
    GameMode gamemode;
} GameOptions;

void init_game(GameState* state, GameOptions* options);
void cleanup_game(GameState* state);
void move_snake(GameState* state, int dx, int dy);
int check_collision(GameState* state);
void generate_food(GameState* state);
int check_food_collision(GameState* state);

#endif
