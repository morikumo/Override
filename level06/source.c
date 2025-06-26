#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ptrace.h>

int auth(char *login, int serial) {
    int i;
    int calculated_serial;
    int len;

    // Retire le retour à la ligne (\n) ajouté par fgets
    login[strcspn(login, "\n")] = '\0';

    // Longueur maximale : 32
    len = strnlen(login, 32);
    if (len <= 5) {
        return 1;  // Échec si login trop court
    }

    // Vérifie si le programme est tracé (debuggé)
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
        puts("\x1B[32m.---------------------------.");
        puts("\x1B[31m| !! TAMPERING DETECTED !!  |");
        puts("\x1B[32m'---------------------------'");
        return 1;  // Échec si debuggage détecté
    }

    // Calcul de base dépendant du 4e caractère du login
    calculated_serial = (login[3] ^ 0x1337) + 6221293;

    // Boucle de transformation du login
    for (i = 0; i < len; ++i) {
        if ((unsigned char)login[i] <= 31) {
            return 1;  // Échec si caractères non imprimables
        }
        calculated_serial += (calculated_serial ^ login[i]) % 1337;
    }

    // Compare le résultat au serial fourni
    return serial != calculated_serial;
}

int main(void) {
    int serial;
    char login[32];
    unsigned int canary = 0;

    // Affichage de bienvenue
    puts("***********************************");
    puts("*\t\tlevel06\t\t  *");
    puts("***********************************");

    // Demande du login
    printf("-> Enter Login: ");
    fgets(login, sizeof(login), stdin);

    puts("***********************************");
    puts("***** NEW ACCOUNT DETECTED ********");
    puts("***********************************");

    // Demande du serial
    printf("-> Enter Serial: ");
    scanf("%d", &serial);

    // Vérification
    if (auth(login, serial)) {
        return 1;
    }

    // Succès
    puts("Authenticated!");
    system("/bin/sh");

    return 0;
}
