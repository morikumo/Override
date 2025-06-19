#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Variable globale pour stocker le nom d'utilisateur saisi
char a_user_name[256]; // taille suffisante pour fgets

/**
 * Vérifie si le nom d'utilisateur est correct.
 * La chaîne attendue est "dat_wil".
 * Renvoie 1 (true) si incorrect, 0 (false) si correct.
 */
int verify_user_name() {
    puts("verifying username....\n");
    return memcmp(a_user_name, "dat_wil", 7) != 0;
}

/**
 * Vérifie si le mot de passe est correct.
 * La chaîne attendue est "admin".
 * Renvoie 1 (true) si incorrect, 0 (false) si correct.
 */
int verify_user_pass(const void *password) {
    return memcmp(password, "admin", 5) != 0;
}

/**
 * Point d’entrée du programme.
 * Affiche une invite de connexion admin.
 * Vérifie successivement le nom d'utilisateur puis le mot de passe.
 */
int main(int argc, const char **argv, const char **envp) {
    char buffer[64]; // Buffer local pour lire le mot de passe
    int login_failed; // Booléen pour stocker l'état de vérification

    memset(buffer, 0, sizeof(buffer));
    login_failed = 0;

    puts("********* ADMIN LOGIN PROMPT *********");

    // Lecture du nom d'utilisateur
    printf("Enter Username: ");
    fgets(a_user_name, sizeof(a_user_name), stdin);
    login_failed = verify_user_name();

    if (login_failed) {
        puts("nope, incorrect username...\n");
    } else {
        // Lecture du mot de passe
        puts("Enter Password: ");
        fgets(buffer, 100, stdin); // ⚠️ dépassement possible ici : buffer = 64 octets, fgets lit jusqu'à 100
        login_failed = verify_user_pass(buffer);
        puts("nope, incorrect password...\n");
    }

    return 1;
}
