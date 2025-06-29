int store_number(int a1)
{
    unsigned int number;
    unsigned int index;

    printf(" Number: ");
    number = get_unum();          // Lire la valeur à stocker

    printf(" Index: ");
    index = get_unum();           // Lire l'index où la stocker

    // Si index est un multiple de 3 ou que le byte de poids fort de "number" vaut 183...
    if (index % 3 == 0 || ((number >> 24) & 0xFF) == 183)
    {
        puts(" *** ERROR! ***");
        puts("   This index is reserved for wil!");
        puts(" *** ERROR! ***");
        return 1;
    }
    else
    {
        *((int*)(a1 + 4 * index)) = number; // Stocker dans le tableau
        return 0;
    }
}

int read_number(int a1)
{
    int index;

    printf(" Index: ");
    index = get_unum();  // Lire l'index

    printf(" Number at data[%u] is %u\n", index, *((int*)(a1 + 4 * index)));
    return 0;
}


int main(int argc, const char **argv, const char **envp)
{
    char storage[400];     // Tableau de 100 entiers
    int number;
    char command[4];       // Stocke le nom de la commande
    int tmp1, tmp2, tmp3, tmp4;
    unsigned int canary = read_canary();

    // Mise à zéro de plusieurs variables
    number = 0;
    *(int*)command = 0;
    tmp1 = tmp2 = tmp3 = tmp4 = 0;

    // On efface les arguments et l’environnement
    while (*argv) memset((void*)*argv++, 0, strlen(*argv));
    while (*envp) memset((void*)*envp++, 0, strlen(*envp));

    // Affichage d’accueil
    puts("----------------------------------------------------");
    puts("  Welcome to wil's crappy number storage service!");
    puts("----------------------------------------------------");
    puts(" Commands:");
    puts("    store - store a number into the data storage");
    puts("    read  - read a number from the data storage");
    puts("    quit  - exit the program");
    puts("----------------------------------------------------");
    puts("   wil has reserved some storage :>");
    puts("----------------------------------------------------");

    // Boucle de traitement des commandes
    while (1)
    {
        printf("Input command: ");
        number = 1;

        fgets(command, 20, stdin);    // 🛑 buffer overflow possible ici ! buffer = 4 octets, mais lecture jusqu’à 19 !
        command[strlen(command) - 1] = 0;

        if (!memcmp(command, "store", 5))
            number = store_number((int)storage);
        else if (!memcmp(command, "read", 4))
            number = read_number((int)storage);
        else if (!memcmp(command, "quit", 4))
            return 0;

        if (number)
            printf(" Failed to do %s command\n", command);
        else
            printf(" Completed %s command successfully\n", command);

        // Reset de variables temporaires
        *(int*)command = tmp1 = tmp2 = tmp3 = tmp4 = 0;
    }
}
