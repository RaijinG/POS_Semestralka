#ifndef TERMINAL_GUI_H
#define TERMINAL_GUI_H

#include "game_logic.h"

void draw_game(GameState* state);
void run_game(GameState* state);
int kbhit();
void init_terminal();
void reset_terminal();

#endif
