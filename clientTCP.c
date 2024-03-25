#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

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
        printf("Vous: ");
        fgets(buffer, 1024, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Enlever le newline du buffer
        send(sock, buffer, strlen(buffer), 0);
        
        if(strcmp(buffer, "fin") == 0) break; // Si le message est "fin", termine la boucle.
    }

    close(sock);
    return 0;
}
