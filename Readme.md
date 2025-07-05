# ✅ Connexion à la VM via SSH – Guide complet

## 📌 Objectif

Pouvoir se connecter en SSH à la VM *OverRide* pour travailler sur les niveaux, récupérer les binaires, et les exploiter localement.

---

## 🧱 Pré-requis

* Avoir une **VM OverRide** fonctionnelle dans **VirtualBox**
* La VM doit écouter sur le port **4242 en SSH**
* Avoir accès à un terminal (Linux/macOS/WSL/Cygwin/Powershell avec OpenSSH…)

---

## 🛠️ Méthode 1 : Bridged Adapter + Redirection de port

### 1. Configuration de VirtualBox

* Allez dans les **paramètres réseau** de la VM
* Sélectionnez :
  → **Adaptateur attaché à :** *Accès par pont (Bridged Adapter)*
  → Cela place la VM sur le **même réseau local** que votre machine hôte

### 2. (Optionnel) Redirection de port

* Dans :
  `Paramètres > Réseau > Avancé > Redirection de port`
  → Ajouter une règle de redirection :

  | Nom      | Protocole | Hôte IP | Port Hôte | IP Invité | Port Invité |
  | -------- | --------- | ------- | --------- | --------- | ----------- |
  | SSH Rule | TCP       |         | `4242`    |           | `4242`      |

### 3. Connexion SSH

```bash
ssh -p 4242 level00@192.168.x.x
```

> Remplacez `192.168.x.x` par l’adresse IP affichée au démarrage de la VM

---

## 🔁 Méthode 2 : Réseau Host-only (Privé)

### 1. Créer un réseau Host-only

* Ouvrir **VirtualBox**
* Aller dans :
  `Fichier` → `Gestionnaire de Réseau Hôte (Network Manager)`
  → Créer un nouveau **Host-only network** (ex: `vboxnet0`)

### 2. Lier ce réseau à la VM

* Dans les **paramètres réseau de la VM** :
  → **Adaptateur 1 :** attaché à *Réseau privé hôte (Host-only Adapter)*

### 3. Connexion

* Démarrer la VM et noter l’**adresse IP affichée par l’ISO** (ex: `192.168.56.x`)
* Connexion SSH :

```bash
ssh -p 4242 level00@192.168.56.x
```

---

## 🔍 Si aucune IP ne fonctionne : Scanner avec `nmap`

### 1. Installer `nmap` (si ce n’est pas déjà fait)

```bash
sudo apt install nmap   # ou brew install nmap sur macOS
```

### 2. Scanner un sous-réseau local pour détecter la VM

```bash
nmap 192.168.56.0/24
```

### Exemple de sortie :

```
Nmap scan report for 192.168.56.101
Host is up (0.62s latency).
Not shown: 999 closed ports
PORT     STATE SERVICE
4242/tcp open  vrml-multi-use
```

→ Connexion SSH :

```bash
ssh -p 4242 level00@192.168.56.101
```

---

## 📁 Transférer les fichiers avec SCP

Une fois connecté, **récupérez les binaires** depuis la VM pour les analyser localement (scp se fait sur la machine local):

```bash
scp -P 4242 level00@192.168.56.101:/home/users/level00/level00 .
```

> Cela copie le fichier `level00` depuis la VM vers votre machine actuelle

---

## 🔧 Exploitation en local (décompilation, analyse…)

* Utilisez vos outils locaux : `gdb`, `ltrace`, `strace`, `radare2`, `Ghidra`, etc.
* Vous pouvez aussi utiliser des décompilateurs en ligne :

🔗 [Dogbolt – Explorateur interactif de compilateurs](https://dogbolt.org/)

