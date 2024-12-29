#include <stdio.h>
#include "gui.h"

int main() {
    if (!init_gui()) {
        fprintf(stderr, "Failed to initialize GUI\n");
        return 1;
    }
    run_game();
    cleanup_gui();
    return 0;
}
