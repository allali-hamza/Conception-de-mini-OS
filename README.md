cat > README.md << 'EOF'
# Mini système d'exploitation

Un mini OS développé en C dans le cadre du projet de Conception de Systèmes d'Exploitation.

## Description

Ce projet implémente les fonctionnalités de base d'un système d'exploitation :
- Gestion de l'écran et de la console
- Gestion des interruptions et de l'horloge
- Changement de contexte et ordonnancement

## Prérequis
```bash
# Installer les outils nécessaires
```bash
sudo apt install gcc-multilib qemu-system-x86
```

## Compilation
```bash
make
```
## Debugging
```bash
# Terminal 1 : Lancer QEMU en mode debug
qemu-system-i386 -kernel kernel.bin -s -S

# Terminal 2 : Lancer GDB
gdb kernel.bin
(gdb) target remote localhost:1234
(gdb) break kernel_start
(gdb) continue
```

### Lancer avec QEMU
```bash
qemu-system-i386 -kernel kernel.bin


## Nettoyage
```bash
make clean
```
