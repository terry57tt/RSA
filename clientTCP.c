#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // Nom d'hôte ou adresse IP du serveur
    char *server_name = "localhost";

    // Numéro de port du serveur
    int portno = 8080;

    // Création d'une socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // Récupération de l'adresse IP du serveur
    server = gethostbyname(server_name);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    // Configuration de l'adresse du serveur
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // Connexion au serveur
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    // Échange de données avec le serveur

    // Fermeture de la connexion
    close(sockfd);

    return 0;
}
