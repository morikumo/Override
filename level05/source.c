#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char **argv, const char **envp) {
    char buffer[100];
    unsigned int i;

    // Lecture de l'entrée utilisateur (max 99 caractères + '\0')
    fgets(buffer, sizeof(buffer), stdin);

    // Convertit toutes les lettres majuscules en minuscules
    for (i = 0; i < strlen(buffer); ++i) {
        if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
            buffer[i] ^= 0x20;  // XOR 0x20 convertit une majuscule ASCII en minuscule
        }
    }

    // Affichage du contenu (potentiellement dangereux si format string dans l'entrée)
    printf(buffer);  // ⚠️ Vulnérable à une attaque de type format string

    exit(0);
}
