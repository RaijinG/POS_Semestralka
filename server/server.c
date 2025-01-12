#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>

#define SERVER_PORT 12345

typedef struct {
    int sockfd;
} ServerInfo;

void *handle_client(void *arg) {
    int *newsockfd = (int *)arg;
    char buffer[256];
    int n;

    bzero(buffer, 256);
    n = read(*newsockfd, buffer, 255);
    if (n < 0) {
        perror("Error reading from socket");
        exit(1);
    }

    n = write(*newsockfd, buffer, strlen(buffer));
    if (n < 0) {
        perror("Error writing to socket");
        exit(1);
    }

    close(*newsockfd);
    free(newsockfd);
    return NULL;
}

void handle_sigint(int sig, siginfo_t *info, void *context) {
    ServerInfo *server_info = (ServerInfo *)info->si_value.sival_ptr;
    if (server_info->sockfd != -1) {
        close(server_info->sockfd);
    }
    printf("Server terminated gracefully.\n");
    exit(0);
}

int main() {
    int newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    ServerInfo server_info;
    server_info.sockfd = -1;

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    server_info.sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_info.sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_info.sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error on binding");
        exit(1);
    }

    listen(server_info.sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(server_info.sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("Error on accept");
            exit(1);
        }

        int *newsockfd_ptr = malloc(sizeof(int));
        if (newsockfd_ptr == NULL) {
            perror("Error allocating memory for newsockfd");
            exit(1);
        }
        *newsockfd_ptr = newsockfd;

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, (void *)newsockfd_ptr);
        pthread_detach(client_thread);
    }

    return 0;
}
