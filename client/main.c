#include <SDL2/SDL.h>
#include "gui.h"

int main() {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!init_gui(&window, &renderer)) {
        return 1;
    }

    run_game(renderer);
    cleanup_gui(window, renderer);

    return 0;
}
