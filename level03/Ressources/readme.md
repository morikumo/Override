# 📘 README - Level03 - Projet Override

## 🔍 Analyse du binaire `level03`

### 🧠 Objectif

Trouver l'input qui permet d'afficher `Congratulations!` et d'obtenir un shell dans le binaire `level03`.

---

## 🧬 Compréhension du programme

Le programme principal contient trois fonctions importantes :

### 1. `main`

```c
int __cdecl main(int argc, const char **argv, const char **envp)
{
  time_t v3 = time(0);
  srand(v3);
  puts("***********************************");
  puts("*\t\tlevel03\t\t**");
  puts("***********************************");
  printf("Password:");
  __isoc99_scanf("%d", &savedregs);
  test(savedregs, 322424845);
  return 0;
}
```

🔹 Le programme lit un entier depuis stdin (via `scanf("%d", &savedregs)`),
🔹 puis appelle la fonction `test(savedregs, 322424845)`.

---

### 2. `test`

```c
int __cdecl test(int a1, int a2)
{
  switch (a2 - a1) {
    case 1: case 2: ... case 21:
      return decrypt(a2 - a1);
    default:
      return decrypt(rand());
  }
}
```

🔹 Le programme calcule `a2 - a1`,
🔹 Si le résultat est entre 1 et 21 (inclus), il appelle `decrypt(k)` avec cette valeur,
🔹 Sinon, il appelle `decrypt` avec une valeur pseudo-aléatoire.

👉 On en déduit que **le seul moyen d'avoir un appel contrôlé à `decrypt(k)` avec k entre 1 et 21 est de fournir un `a1` tel que `a2 - a1 = k`**.

Étant donné que `a2 = 322424845`, on en déduit :

```c
a1 = 322424845 - k
```

---

### 3. `decrypt`

```c
int __cdecl decrypt(char a1)
{
  strcpy(buf, "Q}|u`sfg~sf{}|a3");
  for (int i = 0; i < strlen(buf); ++i)
    buf[i] ^= a1;

  if (!strcmp(buf, "Congratulations!"))
    return system("/bin/sh");
  else
    return puts("\nInvalid Password");
}
```

🔹 La chaîne de départ est chiffrée avec un XOR par un octet `a1`.
🔹 Si le résultat donne "Congratulations!", un shell est lancé.

---

## 🔑 Objectif

Trouver la bonne valeur de `k` (comprise entre 1 et 21) telle que :

```c
xor_decrypt("Q}|u`sfg~sf{}|a3", k) == "Congratulations!"
```

Mais comme on ne connaît pas à l'avance la valeur de `k`, **on est obligé de tester toutes les valeurs de 1 à 21**.

---

## 🧪 Test de toutes les valeurs possibles

Nous avons écrit une ligne de commande pour tester chaque possibilité de `k` de 1 à 21 :

```bash
for k in $(seq 1 21); do echo "[*] Trying k=$k..."; (python -c "print 322424845 - $k"; cat) | ./level03; done
```

🔹 Pour chaque `k`, on calcule `input = 322424845 - k` et on l'envoie au binaire.
🔹 La seule valeur qui affiche `Congratulations!` est obtenue avec `k = 18`, donc :

```bash
python -c 'print 322424845 - 18' | ./level03
```

Donne : `322424827`

---

## ✅ Résultat final

Le mot de passe correct est :

```
322424827
```

Il déclenche un XOR sur la chaîne `"Q}|u\`sfg\~sf{}|a3"\` avec 18, ce qui donne :

```
Congratulations!
```

Et permet donc d’obtenir un shell.

---

