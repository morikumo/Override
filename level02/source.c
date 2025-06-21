#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv, char **envp)
{
    // Déclarations et initialisations
    char password_input[96] = {0};   // Saisie utilisateur du mot de passe
    char username[96] = {0};         // Saisie utilisateur du nom d'utilisateur
    char file_password[48] = {0};    // Contenu réel du fichier .pass (41 caractères max + null)
    FILE *fp = NULL;
    int read_size = 0;

    // Ouverture du fichier contenant le mot de passe
    fp = fopen("/home/users/level03/.pass", "r");
    if (!fp) {
        fwrite("ERROR: failed to open password file\n", 1, 36, stderr);
        exit(1);
    }

    // Lecture de 41 caractères depuis le fichier
    read_size = fread(file_password, 1, 41, fp);
    file_password[strcspn(file_password, "\n")] = '\0';  // Supprime le \n final s’il existe
    fclose(fp);

    if (read_size != 41) {
        fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
        fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
        exit(1);
    }

    // Interface utilisateur
    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\\**************************************/");

    // Lecture du nom d'utilisateur
    printf("--[ Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    // Lecture du mot de passe
    printf("--[ Password: ");
    fgets(password_input, sizeof(password_input), stdin);
    password_input[strcspn(password_input, "\n")] = '\0';

    puts("*****************************************");

    // Comparaison
    if (strncmp(file_password, password_input, 41) != 0) {
        printf(username);  // ⚠️ Vulnérabilité ici (format string)
        puts(" does not have access!");
        exit(1);
    }

    // Accès autorisé
    printf("Greetings, %s!\n", username);
    system("/bin/sh");

    return 0;
}
