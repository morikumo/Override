# README - Exploitation du backup level08

## Contexte

Le programme `level08` sert à copier un fichier donné en argument dans un dossier `./backups/`, en conservant la structure du chemin. Il crée aussi un fichier de log `./backups/.log` où il écrit des messages sur le déroulement de la copie.

---

## Objectif

Utiliser ce programme pour récupérer le fichier `.pass` du niveau suivant, par exemple `/home/users/level09/.pass`, en exploitant le fait que le programme copie ce fichier dans un dossier `./backups/` relatif à son répertoire d’exécution.

---

## Étapes détaillées

### 1. Préparer un dossier de travail accessible

Le programme copie dans `./backups/` depuis son répertoire courant, donc :

* Aller dans un dossier où on a les droits d’écriture (par exemple `/tmp`) :

```bash
cd /tmp
```

* Créer l’arborescence nécessaire pour accueillir la copie :

```bash
mkdir -p backups/home/users/level09
```

* Créer un fichier `.log` vide dans `backups/` car le programme en a besoin pour écrire ses logs :

```bash
touch backups/.log
```

---

### 2. Lancer le programme `level08` en argumentant avec le fichier `.pass` cible

Lancer le programme en donnant le chemin complet du fichier à copier :

```bash
~/level08 /home/users/level09/.pass
```

* `level08` va lire `/home/users/level09/.pass`
* Puis copier ce fichier dans `/tmp/backups/home/users/level09/.pass`

---

### 3. Lire le fichier copié

Tu peux maintenant lire le fichier `.pass` copié dans le dossier `backups` que tu as créé :

```bash
cat backups/home/users/level09/.pass
```

Tu as ainsi récupéré le contenu du fichier secret du niveau suivant.

---

## Pourquoi ça fonctionne ?

* **Le programme lit le fichier source (l’argument) avec les droits de l’utilisateur qui l’exécute.**
* **Il crée une copie du fichier dans un dossier `./backups` situé dans son répertoire courant.**
* En lançant le programme depuis un dossier où tu as le droit d’écrire, tu contrôles où la copie sera créée.
* Tu prépares l’arborescence de dossiers nécessaires dans `./backups` avant de lancer le programme.
* Le fichier `.log` est nécessaire pour que le programme s’exécute sans erreur.
* En combinant tout ça, tu forces le programme à copier un fichier auquel il a accès dans un dossier contrôlé, d’où tu peux facilement lire la copie.

---

## Points importants

* Si tu n’as pas les droits de lecture sur le fichier source, la copie échouera.
* Si les dossiers dans `./backups` ne sont pas présents, la création du fichier échoue.
* Le fichier `./backups/.log` doit exister pour que le programme puisse écrire ses logs.
* Cette méthode ne modifie pas le fichier original, elle fait juste une copie.

---

## Résumé de la commande complète

```bash
cd /tmp
mkdir -p backups/home/users/level09
touch backups/.log
~/level08 /home/users/level09/.pass
cat backups/home/users/level09/.pass
fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
```

---

