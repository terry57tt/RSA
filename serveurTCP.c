#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <stdbool.h>
#include <fcntl.h>
#include <scan_horiz.c>

#define PORT 8080

typedef struct {
    int port;
    bool is_open;
} port_info;

#include <time.h>

void scan_vertical(char *ip_address, int start_port, int end_port, char *buffer) {
    struct sockaddr_in sock_addr;
    socklen_t sock_addr_len = sizeof(sock_addr);
    port_info results[1024] = {0};
    buffer[0] = '\0'; // Initialiser la chaîne de caractères
    bool too_long = false;

    for (int i = start_port; i <= end_port; i++) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("ERROR opening socket");
            exit(1);
        }

        // Set the socket to non-blocking mode
        int flags = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = htons(i);
        inet_pton(AF_INET, ip_address, &sock_addr.sin_addr);

        // Attempt to connect
        if (connect(sockfd, (struct sockaddr *)&sock_addr, sock_addr_len) == -1 && errno != EINPROGRESS) {
            // Handle error
            perror("ERROR connecting to socket");
            close(sockfd);
            continue;
        }

        fd_set writefds;
        struct timeval timeout;

        timeout.tv_sec = 0; // en secondes
        timeout.tv_usec = 5000; // en microsecondes

        // Start the timer
        FD_ZERO(&writefds);
        FD_SET(sockfd, &writefds);
        int select_result = select(sockfd + 1, NULL, &writefds, NULL, &timeout);
        if (select_result == -1) {
            perror("Select error");
            close(sockfd);
            continue;
        } else if (select_result == 0) {
            // Timeout occurred (en général, ca signifie adresse IP invalide)
            too_long = true;
            close(sockfd);
            continue;
        }

        int error;
        socklen_t len = sizeof(error);
        getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
        if (error != 0) {
            // Connection failed
            results[i].port = i;
            results[i].is_open = false;
        } else {
            // Connection successful
            printf("Port %d is open\n", i);
            results[i].port = i;
            results[i].is_open = true;

            // Ajouter les informations sur le port ouvert à la chaîne de caractères
            char port_info_str[50];
            sprintf(port_info_str, "Port %d\n", i);
            strcat(buffer, port_info_str);
        }

        close(sockfd);
    }

    if(too_long) { // Si le temps d'attente est trop long
        strcat(buffer, "Timeout\n");
        printf("Attente trop longue\n");
    } else if (strlen(buffer) == 0) { // Si aucun port n'est ouvert 
        strcat(buffer, "Aucun port ouvert.\n");
        printf("Aucun port ouvert.\n");
    } else { // Afficher les ports ouverts
        printf("Ports ouverts :\n%s\n", buffer);
    }
}


void handleClientRequest(char *request, char *response) {
    // lit juste la première lettre de la requête
    char first_letter = request[0];
    char *ip_address = (char *)malloc(1024);

    switch(atoi(&first_letter)) {
        case 1:
            scan_horizontal(response);
            strcat(response, "Scan horizontal terminé.\n");
            break;
        case 2:
            // si la requête est "2", alors la deuxième lettre est l'adresse IP
            // donc on prend la deuxième lettre jusqu'à la fin de la chaîne
            strcpy(ip_address, request + 1);
            scan_vertical(ip_address, 1, 1024, response);
            free(ip_address);
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
        int valread = read(new_socket, buffer, 1024); // le serveur est bloqué ici jusqu'à ce qu'il reçoive un message

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
