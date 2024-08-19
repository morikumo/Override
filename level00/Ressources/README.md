
---

# Override - Niveau 00

## Description

Ce premier niveau du projet "Override" consiste à exploiter un exécutable binaire qui demande un mot de passe pour accéder au niveau suivant. L'objectif est d'identifier la valeur correcte du mot de passe en analysant le binaire, puis d'obtenir un accès à un shell pour passer au niveau supérieur.

## Analyse de l'exécutable

### Vérification des protections (Checksec)

```bash
$ checksec --file=level00
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
Partial RELRO   No canary found   NX enabled    No PIE          No RPATH   No RUNPATH   /home/users/level00/level00
```

### Configuration de l'environnement

L'exécutable a les permissions suivantes :

```bash
total 13
dr-xr-x---+ 1 level01 level01   60 Sep 13  2016 .
dr-x--x--x  1 root    root     260 Oct  2  2016 ..
-rw-r--r--  1 level01 level01  220 Sep 10  2016 .bash_logout
lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
-rw-r--r--  1 level00 level00 3533 Sep 10  2016 .bashrc
-rwsr-s---+ 1 level01 users   7280 Sep 10  2016 level00
-rw-r--r--  1 level01 level01  675 Sep 10  2016 .profile
```

L'exécutable `level00` a les privilèges de `level01`, ce qui signifie que l'exploit nous permettra d'obtenir un shell avec les droits de `level01`.

### Décompilation et Analyse

Le code décompilé révèle que l'exécutable demande un mot de passe et le compare à une valeur spécifique. Voici le code décompilé :

```c
int main(int argc, const char **argv, const char **envp)
{
  int v4;

  puts("***********************************");
  puts("* \t     -Level00 -\t\t  *");
  puts("***********************************");
  printf("Password:");
  scanf("%d", &v4);
  if ( v4 == 5276 )
  {
    puts("\nAuthenticated!");
    system("/bin/sh");
    return 0;
  }
  else
  {
    puts("\nInvalid Password!");
    return 1;
  }
}
```

La comparaison se fait avec la valeur `5276`. Si cette valeur est correcte, un shell est ouvert.

### Validation avec GDB

Pour valider cette hypothèse, nous avons utilisé GDB pour observer la valeur de comparaison :

```asm
Dump of assembler code for function main:
   0x08048494 <+0>:	push   %ebp
   0x08048495 <+1>:	mov    %esp,%ebp
   0x08048497 <+3>:	and    $0xfffffff0,%esp
   0x0804849a <+6>:	sub    $0x20,%esp
   0x0804849d <+9>:	movl   $0x80485f0,(%esp)
   0x080484a4 <+16>:	call   0x8048390 <puts@plt>
   0x080484a9 <+21>:	movl   $0x8048614,(%esp)
   0x080484b0 <+28>:	call   0x8048390 <puts@plt>
   0x080484b5 <+33>:	movl   $0x80485f0,(%esp)
   0x080484bc <+40>:	call   0x8048390 <puts@plt>
   0x080484c1 <+45>:	mov    $0x804862c,%eax
   0x080484c6 <+50>:	mov    %eax,(%esp)
   0x080484c9 <+53>:	call   0x8048380 <printf@plt>
   0x080484ce <+58>:	mov    $0x8048636,%eax
   0x080484d3 <+63>:	lea    0x1c(%esp),%edx
   0x080484d7 <+67>:	mov    %edx,0x4(%esp)
   0x080484db <+71>:	mov    %eax,(%esp)
   0x080484de <+74>:	call   0x80483d0 <__isoc99_scanf@plt>
   0x080484e3 <+79>:	mov    0x1c(%esp),%eax
   0x080484e7 <+83>:	cmp    $0x149c,%eax
   0x080484ec <+88>:	jne    0x804850d <main+121>
   0x080484ee <+90>:	movl   $0x8048639,(%esp)
   0x080484f5 <+97>:	call   0x8048390 <puts@plt>
   0x080484fa <+102>:	movl   $0x8048649,(%esp)
   0x08048501 <+109>:	call   0x80483a0 <system@plt>
   0x08048506 <+114>:	mov    $0x0,%eax
   0x0804850b <+119>:	jmp    0x804851e <main+138>
   0x0804850d <+121>:	movl   $0x8048651,(%esp)
   0x08048514 <+128>:	call   0x8048390 <puts@plt>
   0x08048519 <+133>:	mov    $0x1,%eax
   0x0804851e <+138>:	leave  
   0x0804851f <+139>:	ret    
End of assembler dump.
```

### Exploitation

En utilisant la valeur `5276` comme mot de passe, nous obtenons l'accès au shell et pouvons ensuite accéder au niveau suivant :

```bash
$ ./level00
***********************************
*          -Level00 -             *
***********************************
Password: 5276

Authenticated!
$ whoami
level01
$ cd /home/users/level01
$ cat .pass
uSq2ehEGT6c9S24zbshexZQBXUGrncxn5sD5QfGL
```


--- 
