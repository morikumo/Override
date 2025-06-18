# 📘 Guide GDB & Assembleur x86 - Apprentissage pour Débutant

Ce document regroupe :
- les **commandes importantes de GDB** (avec alias),
- les **instructions assembleur essentielles** pour comprendre le fonctionnement d’un programme,
- un rappel sur la **mémoire** (stack/heap/code/data),
- une **explication claire des adresses mémoire**.

---

## 🧠 1. Commandes importantes de GDB

| Commande | Alias | Description |
|----------|-------|-------------|
| `run` | `r` | Lance l'exécution du programme |
| `break <addr/fonction>` | `b` | Met un point d’arrêt |
| `continue` | `c` | Continue jusqu’au prochain point d’arrêt |
| `next` | `n` | Exécute l’instruction suivante sans entrer dans les appels |
| `step` | `s` | Exécute l’instruction suivante et entre dans les appels |
| `finish` | — | Termine la fonction en cours et revient à l’appelant |
| `info registers` | `i r` | Affiche l'état des registres |
| `x/<fmt> <adresse>` | — | Examine la mémoire à une adresse (voir formats ci-dessous) |
| `disassemble <function>` | `disas` | Affiche le code assembleur d'une fonction |
| `print <expr>` | `p` | Évalue une expression (C ou registre/valeur) |
| `set <var>` | — | Modifie une variable ou un registre |
| `layout asm` | — | Affiche l'interface en mode visuel assembleur (nécessite `gdb-tui`) |
| `quit` | `q` | Quitte GDB |

### 🔎 Examens mémoire (commande `x`)
Formats :
- `x/4x` : 4 valeurs en hexa
- `x/s` : chaîne de caractères
- `x/10i` : 10 instructions (désassemblées)
- `x/1w` : un mot (4 octets)

---

## ⚙️ 2. Instructions assembleur courantes à connaître (x86 AT&T)

| Instruction | Signification | Usage typique |
|-------------|---------------|----------------|
| `mov src, dst` | Copie `src` dans `dst` | `mov %esp, %ebp` |
| `push reg` | Empile une valeur sur la pile | Sauvegarde de registres |
| `pop reg` | Dépile la valeur du sommet de la pile | Restaure un registre |
| `call addr` | Appelle une fonction (sauve EIP dans la pile) | `call puts` |
| `ret` | Retourne à l'appelant (pop l'adresse retour) | Fin de fonction |
| `cmp a, b` | Compare `a` à `b` (modifie les flags) | `cmp %eax, %ebx` |
| `jmp addr` | Saut inconditionnel | `jmp 0x80483a0` |
| `je`, `jne`, `jg`, `jl`, etc. | Sauts conditionnels (Zero flag, etc.) | Contrôle de flux |
| `lea src, dst` | Calcul d’adresse sans accès mémoire | `lea 0x1c(%esp), %ebx` |
| `add`, `sub`, `inc`, `dec` | Opérations arithmétiques | Calculs simples |
| `and`, `or`, `xor` | Opérations logiques | `and $0xfffffff0, %esp` |
| `nop` | Ne fait rien | Alignement mémoire ou padding |
| `rep stos` | Répète l’instruction `stos` selon `%ecx` | Remplit mémoire (type `memset`) |

---

## 🧷 3. Mémoire en x86 : comprendre les segments

```
+----------------------+ <- Adresse élevée
| Stack (pile)         | <-- %esp, %ebp
| Variables locales    |
| Adresse de retour    |
+----------------------+
| Heap (tas)           | <-- malloc/free (dynamique)
|                      |
+----------------------+
| .bss                 | Variables non initialisées
| .data                | Variables initialisées
| .rodata              | Chaînes constantes
+----------------------+
| .text (code binaire) | <-- contient les instructions
+----------------------+ <- Adresse basse
```

### 📌 Registres liés à la mémoire :
- `ESP` : **Stack Pointer**, sommet de la pile
- `EBP` : **Base Pointer**, base du cadre de pile actuel
- `EIP` : **Instruction Pointer**, adresse de l’instruction courante

---

## 🧾 4. Adresses mémoire : comment les lire

Exemple ligne `gdb` :
```
0x080484d0 <main+0>:    push   %ebp
```

| Élément | Signification |
|---------|---------------|
| `0x080484d0` | Adresse réelle dans la mémoire (segment `.text`) |
| `<main+0>` | Offset de l’instruction dans la fonction |
| `push %ebp` | Instruction assembleur (ici, début du prologue) |

### 🛠 Interpréter une adresse

1. **Si elle commence par `0x0804`**, elle est souvent dans la section `.text` ou `.rodata` (code ou données constantes).
2. **Si elle est proche de `0xffff...`**, elle est dans la pile (`stack`).
3. **Tu peux voir la section d’une adresse avec** :
```bash
readelf -S programme

---

## 🧮 5. Les registres en x86 (32 bits)

Les registres sont des emplacements mémoire très rapides utilisés pour stocker temporairement des données pendant l'exécution des instructions. Voici les principaux registres généraux en architecture x86 :

| Registre | Nom complet | Usage principal |
|----------|-------------|-----------------|
| `EAX`    | Accumulateur | Résultats d'opérations arithmétiques, retours de fonctions |
| `EBX`    | Base         | Utilisation libre, souvent utilisé pour stocker des pointeurs |
| `ECX`    | Counter      | Utilisé comme compteur dans les boucles, instructions répétées (`rep`) |
| `EDX`    | Data         | I/O, multiplication/division longues |
| `ESI`    | Source Index | Source dans les opérations de copie mémoire (`movs`, `stos`) |
| `EDI`    | Dest Index   | Destination dans les opérations de copie mémoire |
| `EBP`    | Base Pointer | Pointe vers le bas de la pile d'une fonction (cadre de pile) |
| `ESP`    | Stack Pointer| Pointe vers le haut de la pile (top stack) |
| `EIP`    | Instruction Pointer | Adresse de la prochaine instruction à exécuter |

### 🔧 Registres spéciaux

- `EFLAGS` : Contient des **flags** qui reflètent l'état du processeur (Zero Flag, Carry, Overflow, etc.).
- `CS`, `DS`, `SS`, etc. : Registres de segment (utilisés pour la segmentation mémoire, moins courant en analyse simple).
- `AX`, `BX`, etc. : Versions 16 bits des registres (`EAX` → `AX` → `AH`/`AL` pour haut/bas octet).

---

### 💡 Tips

- Le registre `EIP` **n'est jamais modifié directement**. Il change après un `call`, `jmp`, `ret` ou instruction normale.
- Tu peux afficher tous les registres avec :
```bash
(gdb) info registers
```

---

## 📚 Bonus : mini aide-mémoire des commandes utiles

```bash
# Lancer et désassembler main
gdb ./a.out
(gdb) disas main

# Mettre un point d'arrêt et lancer
(gdb) break main
(gdb) run

# Exécuter pas à pas
(gdb) next         # instruction suivante
(gdb) step         # entre dans les fonctions

# Voir registres
(gdb) info registers

# Lire la mémoire
(gdb) x/4x $esp    # 4 cases hexa à l'adresse du stack pointer
(gdb) x/s $esp     # chaîne de caractères à l'adresse $esp

# Continuer / quitter
(gdb) continue
(gdb) quit
```

---

## 🧠 À retenir

- La **pile** est essentielle pour comprendre les appels de fonction, variables locales, et adresses de retour.
- Le code assembleur est **proche de la machine**, chaque instruction affecte la mémoire ou les registres.
- Les adresses mémoire sont **organisées par segments** dans un programme ELF.
- Utiliser GDB, c’est **observer le comportement d’un programme en live**, comprendre ce qui se passe instruction par instruction.

---
