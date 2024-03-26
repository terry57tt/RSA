# Fonctionnement

## Format des requêtes entre client et serveur

Les requêtes sont uniquement des chaînes de caractères.

Pour envoyer des informations du client au serveur on envoie sous la forme :
    Client envoie : chiffre|information
    Serveur lit le chiffre et sait quelle information il doit lire.

Pour le scan horizontal, le client envoie : 1|adresse_reseau
Pour le scan vertical, le client envoie : 2|adresse_ip
Pour l'aide, le client envoie : 3
Pour quitter, le client envoie : 4 (signal de fin de connexion)

Pour envoyer des informations du serveur au client on envoie sous la forme :
    Serveur envoie : donnee (pas de mise en forme particulière)
    Client lit la donnee et l'affiche.