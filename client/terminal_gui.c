#include "terminal_gui.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

static struct termios old_tio, new_tio;

void init_terminal() {
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void reset_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
}

void draw_game(GameState* state) {
    system("clear");

    char screen[state->screen_height][state->screen_width];

    for (int i = 0; i < state->screen_height; i++) {
        for (int j = 0; j < state->screen_width; j++) {
            if (i == 0 || i == state->screen_height - 1) {
                screen[i][j] = '-';
            } else if (j == 0 || j == state->screen_width - 1) {
                screen[i][j] = '|';
            } else {
                screen[i][j] = ' ';
            }
        }
    }

    Snake* current = state->snake_head;
    while (current) {
        if (current->y > 0 && current->y < state->screen_height - 1 &&
            current->x > 0 && current->x < state->screen_width - 1) {
            screen[current->y][current->x] = 'O';
            }
        current = current->next;
    }

    screen[state->food.y][state->food.x] = 'X';

    Obstacle* obstacle = state->obstacles;
    while (obstacle) {
        if (obstacle->y > 0 && obstacle->y < state->screen_height - 1 &&
            obstacle->x > 0 && obstacle->x < state->screen_width - 1) {
            screen[obstacle->y][obstacle->x] = '#';
            }
        obstacle = obstacle->next;
    }

    for (int i = 0; i < state->screen_height; i++) {
        for (int j = 0; j < state->screen_width; j++) {
            putchar(screen[i][j]);
        }
        if (i == 1) {
            printf("  Score: %d", state->snake_length - 1);
        }
        putchar('\n');
    }
}

void run_game(GameState* state, GameOptions* options) {
    int dx = 1, dy = 0;
    int running = 1;
    int delay = options->difficulty == Easy ? 300000 : options->difficulty == Normal ? 200000 : 100000;

    init_terminal();
    atexit(reset_terminal);

    while (running) {
        if (kbhit()) {
            char input = getchar();
            switch (input) {
                case 'w': if (dy == 0) { dx = 0; dy = -1; } break;
                case 's': if (dy == 0) { dx = 0; dy = 1; } break;
                case 'a': if (dx == 0) { dx = -1; dy = 0; } break;
                case 'd': if (dx == 0) { dx = 1; dy = 0; } break;
                case 'q': running = 0; break;
            }
        }

        move_snake(state, dx, dy);

        if (check_collision(state)) {
            printf("Game Over\n");
            running = 0;
            return;
        }

        check_food_collision(state);

        if (options->gamemode == Challenge) {
            generate_item(state, 1);
        }

        draw_game(state);
        usleep(delay);
    }
}


int kbhit() {
    int bytes;
    ioctl(STDIN_FILENO, FIONREAD, &bytes);
    return bytes > 0;
}