# Consignes et explications

## Tout d'abord la connection en ssh via la VM

### Etant donné que je suis sur WSL :

Passer en mode "Bridged Adapter" pour que la VM soit sur le même réseau que WSL et avoir une IP type 192.168.x.x (préférable).

Ajouter une redirection de port (Port Forwarding) sur VirtualBox :

Redémarre la VM et teste :

ssh -p 4242 level00@<Ip-addr>

## ////////////////////////////////

## Solution 2 :

Etant donné que l'option précdente n'était pas concluante je suis passer sur du **Host-only network** dans la configuration réseau avec lequel on ajoute un **Host-only network** dans la section :
 **file > tool > Network manager > Host-only network > Create**

Si ça n'est pas 


Lancer la vm et utiliser l'ip que l'iso vous fourni ex: 192.168.x.x

Essayer comme ça :

**ssh -p 4242 level00@< Ip-addr >**

#### Si ça ne marche toujours pas !

Dans ce cas vous recuperer l'ip prefix fourni par l'iso que vous avez en lançant la vm et vous le passer a nmap (nmap n'est pas interne il faut l'installer si ce n'est pas déja fait).

Ex:
**nmap 192.168.56.101**

Exemple de resultat : 

**Nmap scan report for 192.168.56.101**

**Host is up (0.62s latency).**

**Not shown: 999 closed ports**

**PORT     STATE SERVICE**

**4242/tcp open  vrml-multi-use**


## Une fois la connection établie

### Petite consigne importante 

Ne surtout pas oublier d'utiliser la commande scp !  Trés utile pour copier les executables de la VM a la machine local pour exploiter a fond l'executable (décompiler etc..)

Exemple d'utilisation (dans la machine local pâs la vm) :

**scp -P 4242 level00@192.168.56.101:/home/users/level00/level00 .**