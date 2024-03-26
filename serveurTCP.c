#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

#define PORT 8080

typedef struct {
    int port;
    bool is_open;
} port_info;

void scan_vertical(char *ip_address, int start_port, int end_port, port_info *results) {
    struct sockaddr_in sock_addr;
    socklen_t sock_addr_len = sizeof(sock_addr);

    for (int i = start_port; i <= end_port; i++) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("ERROR opening socket");
            exit(1);
        }

        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = htons(i);
        inet_pton(AF_INET, ip_address, &sock_addr.sin_addr);

        if (connect(sockfd, (struct sockaddr *)&sock_addr, sock_addr_len) == 0) {
            results[i].port = i;
            results[i].is_open = true;
            printf("Port %d is open\n", i);
        } else {
            results[i].port = i;
            results[i].is_open = false;
            printf("Port %d is closed\n", i);
        }

        close(sockfd);
    }
}

void handleClientRequest(char *request, char *response) {
    switch(atoi(request)) {
        case 1:
            strcpy(response, "SCAN_H\n");
            break;
        case 2:
            strcpy(response, "SCAN_V\n");
            break;
        case 3:
            strcpy(response, "HELP\n");
            break;
        case 4:
            strcpy(response, "QUIT\n");
            break;
        default:
            strcpy(response, "Commande invalide.\n");
            break;
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Création du socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attacher le socket au port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    while(1) {
        memset(buffer, 0, sizeof(buffer)); // Nettoyer le buffer
        int valread = read(new_socket, buffer, 1024);

        char *response = (char *)malloc(1024);
        handleClientRequest(buffer, response);

        if(valread <= 0) {
            printf("Connexion fermée par le client.\n");
            break;
        }
        printf("Client : %s\n",buffer);

        // Envoyer une réponse au client
        send(new_socket, response, strlen(response), 0);

        if(strcmp(buffer, "fin") == 0) break; // Si le message est "fin", arrêtez le serveur.

        free(response);
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
