#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define SERVER_PORT 12345

void *handle_client(void *arg) {
    int newsockfd = *((int *)arg);
    char buffer[256];
    int n;

    bzero(buffer, 256);
    n = read(newsockfd, buffer, 255);
    if (n < 0) {
        perror("Error reading from socket");
        exit(1);
    }

    printf("Client message: %s\n", buffer);

    snprintf(buffer, sizeof(buffer), "Server response: Game logic running...\n");
    n = write(newsockfd, buffer, strlen(buffer));
    if (n < 0) {
        perror("Error writing to socket");
        exit(1);
    }

    close(newsockfd);
    return NULL;
}

int main() {
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error on binding");
        exit(1);
    }

    listen(sockfd, 5);
    printf("Server is listening on port %d...\n", SERVER_PORT);

    clilen = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("Error on accept");
            exit(1);
        }
        printf("Connection successful\n");

        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, (void *)&newsockfd) < 0) {
            perror("Error creating thread");
            exit(1);
        }
        pthread_detach(client_thread);
    }
}
