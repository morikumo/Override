
## ✅ Exploitation du level06 (Override)

Dans ce niveau, le binaire `level06` appelle un shell (`/bin/sh`) si l’on fournit un *login* et un *serial* qui satisfont certaines conditions internes. Cependant, une protection est mise en place à l'aide de `ptrace` pour empêcher tout débogage du binaire, ce qui rend l’analyse plus difficile.

---

### 🛑 Mécanisme anti-debug : `ptrace`

Le programme utilise `ptrace` pour détecter la présence d’un débogueur. Voici ce qu’il se passe :

* Si `ptrace(PTRACE_TRACEME, ...)` retourne `-1`, cela signifie qu’un débogueur est attaché.
* Le programme détecte alors une triche et quitte immédiatement avec le message :

```
.---------------------------.
| !! TAMPERING DETECTED !!  |
'---------------------------'
```

Heureusement, on peut **contourner cette protection** en interceptant l’appel à `ptrace` avec `gdb` et en **forçant son retour à une valeur différente de `-1`** (par exemple, `1`).

---

### 🎯 Objectif : Bypasser `ptrace` et récupérer la valeur sérialisée

---

### 🔬 Analyse statique (désassemblage de `auth`)

D’après le désassemblage de la fonction `auth` :

```asm
   0x080487b5 <+109>:  call   0x80485f0 <ptrace@plt>        ; Appel de ptrace
   0x080487ba <+114>:  cmp    $0xffffffff, %eax             ; Vérifie s’il retourne -1
   ...
   0x08048866 <+286>:  cmp    -0x10(%ebp), %eax             ; Comparaison du serial attendu
```

La logique du programme est simple : la fonction `auth` retourne le résultat de `a2 != v4`.
Il faut donc s'assurer que la **valeur calculée (`v4`) à partir du login** soit **égale au serial** fourni en entrée.

---

### 🧪 Étapes dans GDB

1. **Lancer GDB sur le programme :**

```bash
gdb -q ./level06
```

2. **Placer deux breakpoints** :

* Au moment où `ptrace` est appelé (pour le forcer à réussir)
* Juste avant la comparaison entre `v4` et `a2`

```gdb
(gdb) b *auth+114          # Après l’appel à ptrace, juste avant la vérification
(gdb) b *auth+286          # Comparaison entre serial attendu et calculé
(gdb) r                    # Démarre le programme
```

---

### 🧰 Exécution et modification de ptrace

Lors de l’exécution :

```text
-> Enter Login: abcdef
-> Enter Serial: 1234
```

Quand le premier breakpoint est atteint (`auth+114`) :

```gdb
(gdb) set $eax=1           # On modifie le retour de ptrace pour qu’il ne retourne pas -1
(gdb) continue
```

---

### 🔍 Extraction de la valeur sérialisée

Quand le deuxième breakpoint est atteint (`auth+286`) :

```gdb
(gdb) x/wx $ebp-0x10       # Affiche la variable v4 (valeur calculée à partir du login)
0xffffd698:     0x005f1ae2
(gdb) p/d 0x005f1ae2       # Conversion en décimal
$1 = 6232802
```

💡 La valeur `6232802` est donc la "forme sérialisée" du login `abcdef`.

---

### ✅ Test final

```bash
level06@OverRide:~$ ./level06
-> Enter Login: abcdef
-> Enter Serial: 6232802
Authenticated!
$ cat /home/users/level07/.pass
GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8
```

---

## 🎉 Passage au level07 réussi !

---
