# Consignes et explications

## Tout d'abord la connection en ssh via la VM

### Etant donné que je suis sur WSL :

Passer en mode "Bridged Adapter" pour que la VM soit sur le même réseau que WSL et avoir une IP type 192.168.x.x (préférable).

Ajouter une redirection de port (Port Forwarding) sur VirtualBox :

Redémarre la VM et teste :

ssh -p 4242 level00@<Ip-addr>