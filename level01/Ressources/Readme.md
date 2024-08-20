Ok on arrive avec cette configuration :

```
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
Partial RELRO   No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/users/level01/level01
```

### Sécurité du Binaire `/home/users/level01/level01`

- **Partial RELRO** : Protection partielle contre les attaques sur la table GOT, mais vulnérable aux exploitations ciblant cette zone.
- **No Stack Canary** : Aucune protection contre les dépassements de tampon de la pile, augmentant le risque d'exploits.
- **NX Disabled** : La mémoire n'est pas protégée contre l'exécution de code injecté, facilitant les attaques.
- **No PIE** : Les adresses sont fixes, ce qui rend les attaques basées sur les adresses plus simples.

Ce binaire présente plusieurs faiblesses de sécurité, le rendant vulnérable à différentes exploitations.


