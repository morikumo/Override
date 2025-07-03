#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int secret_backdoor() {
    char command[128];
    // Lit une commande depuis l'entrée standard (stdin)
    fgets(command, sizeof(command), stdin);
    // Exécute la commande système récupérée
    return system(command);
}

int set_username(char *buffer) {
    char username[140];
    int i;

    memset(username, 0, sizeof(username));
    puts(">: Enter your username");
    printf(">>: ");
    fgets(username, sizeof(username), stdin);

    // Copie jusqu'à 40 caractères du username dans buffer + offset 140
    for (i = 0; i <= 40 && username[i]; i++) {
        buffer[140 + i] = username[i];
    }
    // Affiche un message de bienvenue personnalisé
    return printf(">: Welcome, %s", buffer + 140);
}

char *set_message(char *buffer) {
    char message[1024];

    memset(message, 0, sizeof(message));
    puts(">: Msg @Unix-Dude");
    printf(">>: ");
    fgets(message, sizeof(message), stdin);

    // Copie la chaîne message dans buffer en limitant la taille à *(int *)(buffer + 180)
    return strncpy(buffer, message, *(int *)(buffer + 180));
}

int handle_msg() {
    char buffer[140];
    long long zero1 = 0, zero2 = 0, zero3 = 0, zero4 = 0, zero5 = 0;
    int max_len = 140;

    // Initialisation du buffer (rempli plus bas via set_username et set_message)
    memset(buffer, 0, sizeof(buffer));

    // Positionne max_len à 140 dans buffer + 180
    *(int *)(buffer + 180) = max_len;

    // Appelle la saisie du nom d'utilisateur
    set_username(buffer);

    // Appelle la saisie du message
    set_message(buffer);

    puts(">: Msg sent!");
    return 0;
}

int main(int argc, const char **argv, const char **envp) {
    puts("--------------------------------------------");
    puts("|   ~Welcome to l33t-m$n ~    v1337        |");
    puts("--------------------------------------------");

    handle_msg();

    return 0;
}