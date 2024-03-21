# Compte rendu : TP Protocole TCP

Le serveur ouvre une socket, lie la socket à l'adresse locale sur l'un de ses ports et se met en écoute de demandes de connexion.
Le client ouvre une socket et se connecte au serveur.
Le serveur accepte la connexion et le dialogue peut commencer avec le client.
Code de l'application
Le code fourni comprend deux parties : le code du client (clientTCP.c) et le code du serveur (serveurTCP.c).

## Code du client
Dans le code du client, les étapes suivantes sont effectuées :

Création d'une socket avec la fonction socket().

``` sockfd = socket(AF_INET, SOCK_STREAM, 0); ```

Récupération de l'adresse IP du serveur avec la fonction gethostbyname().

``` server = gethostbyname(server_name); ```

Configuration de l'adresse du serveur en remplissant la structure ``` sockaddr_in ``` avec les informations nécessaires (famille d'adresses, adresse IP et numéro de port).

```
serv_addr.sin_family = AF_INET;
bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
serv_addr.sin_port = htons(portno);
```

Connexion au serveur avec la fonction connect().

```
if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR connecting");
    exit(1);
}
```
Échange de données avec le serveur.
Fermeture de la connexion avec la fonction close().

```
close(sockfd);
```

## Code du serveur

Dans le code du serveur, les étapes suivantes sont effectuées :

Création d'une socket avec la fonction socket().

```
if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
}
```

Configuration de l'adresse de la socket en remplissant la structure sockaddr_in avec les informations nécessaires (famille d'adresses, adresse IP et numéro de port).

```
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(PORT);
```

Liaison de la socket à l'adresse configurée avec la fonction ``` bind() ```.

```
if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
}
```
Mise en écoute de la socket avec la fonction ```listen()```.
```
if (listen(server_fd, MAX_CLIENTS) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
}
```
Acceptation des connexions entrantes avec la fonction ```accept()``` dans une boucle infinie.

```
if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
perror("accept");
exit(EXIT_FAILURE);
}
```

Envoi d'un message de bienvenue au client nouvellement connecté avec la fonction ```send()```.

```
send(new_socket, "Bienvenue sur le serveur !\n", strlen("Bienvenue sur le serveur !\n"), 0);
```

Réception et envoi des messages du client dans une boucle infinie jusqu'à ce que le client se déconnecte.
```
while(1) {
    memset(buffer, 0, BUFFER_SIZE);
    int valread = read( new_socket, buffer, BUFFER_SIZE);
    if(valread == 0) {
        printf("Client déconnecté\n");
        break;
    }
    printf("Client : %s\n", buffer);
    send(new_socket, buffer, strlen(buffer), 0);
}
```

Fermeture de la connexion avec la fonction close() (non implémentée dans le code fourni).

