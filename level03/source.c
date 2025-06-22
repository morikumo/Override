#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void decrypt(char key) {
    char encrypted[] = "Q}|u`sfg~sf{}|a3";
    size_t len = strlen(encrypted);

    // Déchiffrement par XOR avec la clé
    for (size_t i = 0; i < len; ++i) {
        encrypted[i] ^= key;
    }

    if (strcmp(encrypted, "Congratulations!") == 0) {
        system("/bin/sh");
    } else {
        puts("\nInvalid Password");
    }
}

void test(int input, int reference) {
    int diff = reference - input;

    switch (diff) {
        case 1: case 2: case 3: case 4: case 5:
        case 6: case 7: case 8: case 9:
        case 16: case 17: case 18: case 19: case 20: case 21:
            decrypt((char)diff);
            break;
        default:
            decrypt((char)(rand() % 256));
            break;
    }
}

int main(int argc, const char **argv, const char **envp) {
    int user_input;
    time_t now = time(NULL);

    srand(now);

    puts("***********************************");
    puts("*\t\tlevel03\t\t**");
    puts("***********************************");
    printf("Password: ");

    scanf("%d", &user_input);

    test(user_input, 322424845);

    return 0;
}
