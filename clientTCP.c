#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>

#define PORT 8080

// vérifie la validité de l'adresse IP
bool check_ip_address(char *ip_address) {
    ip_address[strcspn(ip_address, "\n")] = 0;
    struct in_addr addr;
    int valid = inet_pton(AF_INET, ip_address, &addr);
    return valid != 0;
}

// Afficher le menu (true si on doit envoyer un message, false sinon (commande locale))
bool displayMenu(char *buffer, char *ip_address, int sock) {
    printf("Menu:\n");
    printf("1. Scan horizontal\n");
    printf("2. Scan vertical\n");
    printf("3. Help\n");
    printf("4. Quitter\n");


    fgets(buffer, 1024, stdin);
    buffer[strcspn(buffer, "\n")] = 0; // Enlever le newline du buffer

    switch(atoi(buffer)) {
        case 1:
            strcpy(buffer, "1");
            return true;
        case 2:
            printf("Entrez l'adresse IP : "); // demander l'adresse IP à l'utilisateur
            fgets(ip_address, 1024, stdin);

            if (check_ip_address(ip_address)){
                // buffer prend 2, suivi de l'adresse IP
                strcpy(buffer, "2");
                strcat(buffer, ip_address);
                return true;
            } else {
                printf("Adresse IP invalide (format : xxx.xxx.xxx.xxx).\n");
                return false;
            }
        case 3:
            printf("\nScan horizontal: Scanne les ports d'une adresse IP donnée.\n");
            printf("Scan vertical: Scanne les ports d'une adresse IP donnée.\n");
            printf("Help: Affiche le menu.\n");
            printf("Quitter: Quitte le programme.\n\n");
            return false;
        case 4:
            close(sock);
            return true;
        default:
            printf("Commande invalide.\n");
            return false;
    }
}

int main() {
    struct sockaddr_in serv_addr;
    char buffer[16384] = {0};
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

    // debut de la conversation
    while(1) {
        char* ip_address = malloc(1024);

        if(displayMenu(buffer,ip_address,sock)) { // afficher le menu
            send(sock, buffer, strlen(buffer), 0); // envoi de la requête au serveur

            memset(buffer, 0, sizeof(buffer)); // clear buffer
            int valread = read(sock, buffer, 1024); // attendre la réponse du serveur
            if(valread <= 0) {
                printf("Connexion fermée par le serveur.\n");
                break;
            }
            printf("Serveur : %s\n", buffer);
        }
    
        free(ip_address);        
    }

    close(sock);
    return EXIT_SUCCESS;
}
