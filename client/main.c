#include <SDL_render.h>
#include <SDL_video.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "gui.h"

#define SERVER_PORT 12345

void start_server() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Starting server...\n");
        if (execl("./server", "server", NULL) == -1) {
            perror("execl failed to start the server");
            exit(1);
        }
    } else if (pid < 0) {
        perror("Failed to fork server process");
        exit(1);
    }
    sleep(3);
}

void start_client() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[256];

    sleep(3);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    printf("Attempting to connect to server at %s:%d...\n", inet_ntoa(server_addr.sin_addr), SERVER_PORT);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(1);
    }

    snprintf(buffer, sizeof(buffer), "Client connected\n");
    write(sockfd, buffer, strlen(buffer));

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!init_gui(&window, &renderer)) {
        close(sockfd);
        return;
    }

    run_game(renderer);

    cleanup_gui(window, renderer);
    close(sockfd);
}

int main() {
    start_server();
    start_client();

    return 0;
}
