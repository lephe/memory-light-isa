# ASM 2017 : Rendu 2 (Alain Delaët-Tixeuil et Sébastien Michelland)

Les programmes demandés sont dans /prog, et leurs versions compilées au format
.bin s'y trouvent aussi. En particulier :

    lib_draw.s   Fonctions clear_screen, plot, draw et fill
    lib_font.s   Fonctions putc et puts
    drawing.s    Programme de test pour les fonctions précédentes
    Makefile     Compiler tout /prog au format binaire

Le fichier remarques.txt a été mis à jour avec des remarques sur la gestion de
la mémoire.

## Tour rapide du rendu

Le programme de test inclut les deux bibliothèques par une directive .include.
On peut donc le compiler directement (au format binaire de l'émulateur) :

    ./asm.py -b prog/drawing.s

L'assembleur a été porté pour Python 3.5, mais en cas de doute, le fichier
prog/drawing.bin est fourni avec l'archive. Pour lancer le programme assemblé
dans l'émulateur en console :

    emu/emu -r -g prog/drawing.bin

Pour utiliser le debugger graphique (terminal plein écran recommandé) :

    emu/emu -d -g prog/drawing.bin

Parmi les commandes du debugger, `r` exécute tout le programme, `c` affiche
quelques statistiques d'exécution et `m 10000` dumpe la VRAM en hexadécimal.
Voyez `help` pour le détail du reste (exécution pas-à-pas, breakpoints, ...).

Le code fourni dans les deux fichiers lib_draw.s et lib_font.s respecte les
conventions d'appel décrites dans doc/prog_calling_conventions.md. Le fichier
prog/wrapper.s fournit des wrappers qui utilisent celles du sujet, qui sont
légèrement différentes.

Les labels sont normalement générés automatiquement de façon optimale, et les
fonctions clear_screen et fill ont été optimisées de façon simple.

## Assembleur (/compiler)

`./asm.py --help` affiche l'aide. La commande est de la forme :

	./asm.py [-h] [--stdout | -o FILE] (-m | -t | -b) [--huffman] SOURCE

Le switch --stdout envoie la sortie dans le terminal, tandis que -o écrit dans
un fichier (par défaut). Les options -m, -t et -b spécifient le format :
mnémoniques, texte ou binaire. L'émulateur rendu ici ne supporte que le format
binaire ; le format officiel est le texte.

Il y a une option supplémentaire, --huffman, qui recalcule l'arbre de Huffman
pour optimiser spécifiquement le programme assemblé et produit un fichier
opcode.txt avec le résultat de l'analyse.


## Émulateur (/emu)

Pour compiler l'émulateur, lancez `make` dans le dossier /emu. On a eu des
problèmes d'ingénierie, veuillez voir /doc/emu_troubleshooting.md si quelque
chose se passe mal à un moment (erreur de linkage, mauvais rendu à l'écran...).
L'émulateur est invoqué avec une commande de la forme :

	./emu [-r | -d] [-g] BINARY [options...]

Il lui faut un fichier binaire généré par le mode -b de l'assembleur. Les
fichiers texte seront acceptés mais pas compris ; fournir un fichier texte est
une erreur.

L'émulateur supporte actuellement deux modes d'exécution :
- Le mode -r lance rapidement l'exécution et affiche l'état du processeur
  lorsque le programme se termine. (Par défaut)
- Le mode -d lance le debugger graphique, qui fournit l'exécution pas-à-pas,
  un désassembleur, et de quoi surveiller les registres à tout instant.
- L'option -g, en plus d'une des deux autres, active le mode graphique et lance
  la SDL pour afficher les contenus de l'écran.

L'option --geometry customise la disposition de la mémoire ; normalement il n'y
en a pas besoin. Les détails sont dans /doc/emu_memory_layout.md, si jamais.

Le debugger tourne dans une interface ncurses. La taille minimale utilisable de
terminal est 96 × 32 ; je recommande à partir de 128 × 40, sinon des infos sont
cachées. Le debugger est contrôlé par une ligne de commande qui a une aide
intégrée ; vous pouvez vous y référer.
