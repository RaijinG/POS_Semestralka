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
GameOptions options;

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
    init_game(&state, &options);
    run_game(&state, &options);
    cleanup_game(&state);
    close(sockfd);
    return NULL;
}

void show_main_menu() {
    int choice;
    while (1) {
        printf("\n--- Main Menu ---\n");
        printf("Current Settings:\n");
        printf("Difficulty: ");
        switch (options.difficulty) {
            case Easy: printf("Easy\n"); break;
            case Normal: printf("Medium\n"); break;
            case Hard: printf("Hard\n"); break;
        }

        printf("Map Size: ");
        switch (options.map_size) {
            case Small: printf("Small\n"); break;
            case Medium: printf("Medium\n"); break;
            case Large: printf("Large\n"); break;
        }

        printf("Game Mode: ");
        switch (options.gamemode) {
            case Classic: printf("Classic\n"); break;
            case Challenge: printf("Challenge\n"); break;
        }

        printf("\n1. Play the game\n");
        printf("2. Choose difficulty\n");
        printf("3. Choose the size of the map\n");
        printf("4. Choose the gamemode\n");
        printf("5. Exit\n");
        printf("Select an option: ");
        scanf("%d", &choice);

        if (choice == 1) {
            pthread_t server_thread;
            pthread_t game_logic_thread;

            pthread_create(&server_thread, NULL, server_communication_thread, NULL);
            pthread_create(&game_logic_thread, NULL, game_thread, NULL);

            pthread_join(server_thread, NULL);
            pthread_join(game_logic_thread, NULL);
            break;
        } else if (choice == 2) {
            printf("1. Easy\n2. Medium\n3. Hard\n");
            printf("Select difficulty: ");
            scanf("%d", &choice);
            options.difficulty = choice;
        } else if (choice == 3) {
            printf("1. Small\n2. Medium\n3. Large\n");
            printf("Select map size: ");
            scanf("%d", &choice);
            options.map_size = choice;
        } else if (choice == 4) {
            printf("1. Classic\n2. Challenge\n");
            printf("Select gamemode: ");
            scanf("%d", &choice);
            options.gamemode = choice;
        } else if (choice == 5) {
            printf("Exiting...\n");
            exit(0);
        } else {
            printf("Invalid option, try again.\n");
        }
    }
}

int main() {
    options.difficulty = Medium;
    options.map_size = Medium;
    options.gamemode = Classic;

    show_main_menu();

    return 0;
}