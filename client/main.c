#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include "game_logic.h"
#include "terminal_gui.h"

#define SERVER_PORT 12345

int sockfd;

void *server_communication_thread(void *arg) {
    struct sockaddr_in server_addr;
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(1);
    }

    snprintf(buffer, sizeof(buffer), "Client connected\n");
    write(sockfd, buffer, strlen(buffer));

    return NULL;
}

void *game_thread(void *arg) {
    GameState state;
    init_game(&state);
    run_game(&state);
    cleanup_game(&state);
    close(sockfd);
    return NULL;
}

int main() {

    pthread_t server_thread;
    pthread_t game_logic_thread;

    pthread_create(&server_thread, NULL, server_communication_thread, NULL);
    pthread_create(&game_logic_thread, NULL, game_thread, NULL);

    pthread_join(server_thread, NULL);
    pthread_join(game_logic_thread, NULL);

    return 0;
}
