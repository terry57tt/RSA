# Fonctionnement

## Lancement du scanner

```
make all
./cmake-build-debug/serveurTCP
./cmake-build-debug/clientTCP
```
## Format des requêtes entre client et serveur

Les requêtes sont uniquement des **chaînes de caractères**.

Pour envoyer des informations du client au serveur on envoie sous la forme :

    Client envoie : chiffre|information
    Serveur lit le chiffre et sait comment traiter l'information.

* Pour le scan horizontal, le client envoie : **1**
* Pour le scan vertical, le client envoie : **2|adresse_ip**
* Pour l'aide, le client envoie : **3**
* Pour quitter, le client envoie : **4** (signal de fin de connexion)

Pour envoyer des informations du serveur au client on envoie sous la forme :

    Serveur envoie : donnée (pas de mise en forme particulière)  
    Client lit la donnée et l'affiche.
## Scanner horizontal



## Scanner vertical

Ce scanner vertical va balayer **les ports pour une adresse IP donnée**. Ce scan consiste à **vérifier l'état de chaque port** dans une plage donnée (par défaut de 1 à 1024) pour une adresse IP spécifique.
### Lecture et vérification de l'adresse IP (coté client)

Tout d'abord, le programme vérifie le **format de l'adresse IP** entrée par l'utilisateur. Cette vérification est effectuée en essayant de convertir l'adresse IP en un format numérique. Si la **conversion réussit**, cela signifie que l'adresse IP est au format `x.x.x.x` avec `x` compris entre 0 et 255. Si l'adresse est valide, le programme envoie au serveur la commande `2|xxx.xxx.xxx.xxx`. Si l'adresse **n'est pas valide**, le programme demande à l'utilisateur d'entrer une nouvelle adresse.
### Balayage des ports (coté serveur)

Le serveur **traite ensuite la demande** en appelant la fonction `scan_vertical`. Cette fonction prend en paramètres **l'adresse IP à scanner, le port de départ et le port de fin de la plage à scanner, ainsi qu'un buffer pour stocker les résultats du scan**. Par défaut, la fonction scanne les ports de 1 à 1024.

La fonction `scan_vertical` effectue ensuite le scan des ports en utilisant une boucle qui itère **sur chaque port de la plage à scanner**. Pour chaque port, la fonction crée un **socket**, le configure en **mode non-bloquant**, et essaye de se connecter au serveur à l'adresse IP et au port spécifiés. Si la **connexion échoue**, le port est considéré comme **fermé**. Si la **connexion réussit**, le port est considéré comme **ouvert** et les informations sur le port ouvert sont ajoutées à la chaîne de caractères `buffer`.
### Gestion des délais d'attente (coté serveur)

La fonction `scan_vertical` gère également le **délai d'attente du scan**. Si le délai est trop long, cela signifie généralement que l'adresse IP est invalide et la fonction arrête le scan et retourne un message de timeout. Si le délai n'est pas dépassé, la fonction poursuit le scan des ports.
### Réception et affichage des données (coté client)

La liste des ports ouverts est envoyée au client et celui ci l'affiche telle que reçue pour l'utilisateur. Ensuite, l'utilisateur peut choisir dans le menu l'action à réaliser.