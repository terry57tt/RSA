#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

int displayMenu(char *buffer) {
    printf("Menu:\n");
    printf("1. Scan horizontal\n");
    printf("2. Scan vertical\n");
    printf("3. Help\n");
    printf("4. Quitter\n");

    fgets(buffer, 1024, stdin);
    buffer[strcspn(buffer, "\n")] = 0; // Enlever le newline du buffer

    switch(atoi(buffer)) {
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            printf("\nScan horizontal: Scanne les ports d'une adresse IP donnée.\n");
            printf("Scan vertical: Scanne les ports d'une adresse IP donnée.\n");
            printf("Help: Affiche le menu.\n");
            printf("Quitter: Quitte le programme.\n\n");
            break;
        case 4:
            strcpy(buffer, "fin");
            break;
        default:
            printf("Commande invalide.\n");
            break;
    }
}

int main() {
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    int sock = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convertir l'adresse IPv4 et IPv6 de texte à binaire
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Envoie des messages de manière répétée
    while(1) {
        displayMenu(buffer);
        
        send(sock, buffer, strlen(buffer), 0);
        
        if(strcmp(buffer, "fin") == 0) break; // Si le message est "fin", termine la boucle.

        // Recevoir la réponse du serveur
        memset(buffer, 0, sizeof(buffer)); // Nettoyer le buffer
        int valread = read(sock, buffer, 1024);
        if(valread <= 0) {
            printf("Connexion fermée par le serveur.\n");
            break;
        }
        printf("Serveur : %s\n", buffer);
    }

    close(sock);
    return 0;
}
