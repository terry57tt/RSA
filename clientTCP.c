#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Vous: ");

    while(1) {
        fgets(buffer, 1024, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if(strcmp(buffer, "v") == 0){
            char ip_address[16];
            printf("Enter the IP address to scan: ");
            fgets(ip_address, sizeof(ip_address), stdin);
            ip_address[strcspn(ip_address, "\n")] = '\0';

            int start_port, end_port;
            printf("Enter the range of ports to scan (start end): ");
            scanf("%d %d", &start_port, &end_port);

            port_info results[end_port - start_port + 1]; // Tableau pour stocker les résultats

            scan_vertical(ip_address, start_port, end_port, results);

            // Construction de la chaîne de caractères pour les résultats
            char buffer[1024];
            for (int i = 0; i <= end_port - start_port; i++) {
                char port_status[30];
                snprintf(port_status, sizeof(port_status), "%d %s", (unsigned int)results[i].port, results[i].is_open ? "Open" : "Closed");
                strcat(buffer, port_status);
            }
            buffer[strcspn(buffer, "\n")] = 0;

            send(sock, buffer, strlen(buffer), 0); // Envoi des résultats au serveur
        }
        else if(strcmp(buffer, "fin") == 0) break;
        else {
            send(sock, buffer, strlen(buffer), 0);
            printf("Vous: ");};
    }

    close(sock);
    return 0;
}

