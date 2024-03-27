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

#define PORT 8080

typedef struct {
    int port;
    bool is_open;
} port_info;

void scan_vertical_UDP(char *ip_address, int start_port, int end_port, char *buffer) {
    struct sockaddr_in sock_addr, src_addr;
    socklen_t sock_addr_len = sizeof(sock_addr), src_addr_len = sizeof(src_addr);
    port_info results[1024] = {0};
    buffer[0] = '\0'; // Initialiser la chaîne de caractères
    bool too_long = false;

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Utiliser SOCK_DGRAM pour UDP
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    for (int i = start_port; i <= end_port; i++) {
        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = htons(i);
        inet_pton(AF_INET, ip_address, &sock_addr.sin_addr);

        // Envoyer un datagramme vide
        char empty_datagram[1] = {0};
        if (sendto(sockfd, empty_datagram, sizeof(empty_datagram), 0, (struct sockaddr *)&sock_addr, sock_addr_len) < 0) {
            perror("ERROR sending datagram");
            continue;
        }

        // Réinitialiser le timeout
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 5000;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        char recv_buffer[1024];
        ssize_t recv_len = recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&src_addr, &src_addr_len);

        if (recv_len < 0) {
            if (errno != EWOULDBLOCK) {
                perror("ERROR receiving datagram");
            }
        } else {
            // Vérifier le type de réponse (UDP ou ICMP)
            if (src_addr.sin_port == htons(i)) {
                // Réponse UDP, le port est ouvert
                results[i].port = i;
                results[i].is_open = true;

                // Ajouter les informations sur le port ouvert à la chaîne de caractères
                char port_info_str[50];
                sprintf(port_info_str, "Port %d\n", i);
                strcat(buffer, port_info_str);
            } else {
                // Réponse ICMP, le port est fermé
                printf("Port %d is closed\n", i);
            }
        }
    }

    close(sockfd);

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
            strcpy(response, "SCAN_H\n");
            break;
        case 2:
            // si la requête est "2", alors la deuxième lettre est l'adresse IP
            // donc on prend la deuxième lettre jusqu'à la fin de la chaîne
            strcpy(ip_address, request + 1);
            scan_vertical_UDP(ip_address, 1, 1024, response);
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
