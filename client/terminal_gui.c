#include "terminal_gui.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 20
#define SNAKE_CHAR 'O'
#define FOOD_CHAR 'X'
#define EMPTY_CHAR ' '

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

    char screen[SCREEN_HEIGHT][SCREEN_WIDTH];

    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            if (i == 0 || i == SCREEN_HEIGHT - 1) {
                screen[i][j] = '-';
            } else if (j == 0 || j == SCREEN_WIDTH - 1) {
                screen[i][j] = '|';
            } else {
                screen[i][j] = EMPTY_CHAR;
            }
        }
    }

    Snake* current = state->snake_head;
    while (current) {
        if (current->y > 0 && current->y < SCREEN_HEIGHT - 1 &&
            current->x > 0 && current->x < SCREEN_WIDTH - 1) {
            screen[current->y][current->x] = SNAKE_CHAR;
        }
        current = current->next;
    }

    screen[state->food.y][state->food.x] = FOOD_CHAR;

    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            putchar(screen[i][j]);
        }
        putchar('\n');
    }
}

void run_game(GameState* state) {
    int dx = 1, dy = 0;
    int running = 1;

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
        }

        if (check_food_collision(state)) {
            printf("Score: %d\n", state->snake_length);
        }

        draw_game(state);
        usleep(200000);
    }
}

int kbhit() {
    int bytes;
    ioctl(STDIN_FILENO, FIONREAD, &bytes);
    return bytes > 0;
}