#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

unsigned long long log_wrapper(FILE *logfile, const char *prefix, const char *msg) {
    char buffer[264];
    unsigned long long cookie = 0;

    // Simule une protection d'intégrité (Stack Canary) - on l'ignore ici
    cookie = 0; 

    // Copie le préfixe dans buffer
    strcpy(buffer, prefix);

    // Ajoute le message à la suite, en limitant la taille pour éviter dépassement
    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "%s", msg);

    // Supprime le saut de ligne s'il y en a un à la fin
    buffer[strcspn(buffer, "\n")] = '\0';

    // Écrit dans le fichier log
    fprintf(logfile, "LOG: %s\n", buffer);

    // Retourne la valeur simulée (inutile ici)
    return cookie;
}

int main(int argc, const char **argv) {
    FILE *logfile;
    FILE *sourcefile;
    int fd_dest;
    char c;
    char dest_path[104];

    if (argc != 2) {
        printf("Usage: %s filename\n", argv[0]);
        return 1;
    }

    // Ouvre le fichier de log (toujours ./backups/.log)
    logfile = fopen("./backups/.log", "w");
    if (!logfile) {
        printf("ERROR: Failed to open %s\n", "./backups/.log");
        exit(1);
    }

    // Log de début
    log_wrapper(logfile, "Starting back up: ", argv[1]);

    // Ouvre le fichier source à copier (chemin donné en argument)
    sourcefile = fopen(argv[1], "r");
    if (!sourcefile) {
        printf("ERROR: Failed to open %s\n", argv[1]);
        exit(1);
    }

    // Prépare le chemin de destination dans ./backups/
    strcpy(dest_path, "./backups/");
    strncat(dest_path, argv[1], sizeof(dest_path) - strlen(dest_path) - 1);

    // Ouvre le fichier destination en écriture, crée-le s'il n'existe pas
    fd_dest = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0640);
    if (fd_dest < 0) {
        printf("ERROR: Failed to open %s\n", dest_path);
        exit(1);
    }

    // Copie caractère par caractère du sourcefile vers fd_dest
    while ((c = fgetc(sourcefile)) != EOF) {
        write(fd_dest, &c, 1);
    }

    // Log de fin
    log_wrapper(logfile, "Finished back up ", argv[1]);

    // Fermeture des fichiers
    fclose(sourcefile);
    close(fd_dest);
    fclose(logfile);

    return 0;
}
