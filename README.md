# ASM 2017 : Rendu 1 (Alain Delaët-Tixeuil et Sébastien Michelland)

Les programmes demandés sont dans /prog. Comme nos registres font 64 bits, les
tailles ont été ajustées. On trouve donc :

    mult.s     Multiplication non signée 64 × 64 → 64 bits
    muls.s     Multiplication signée 64 × 64 → 64 bits
    mull.s     Multiplication signée 64 × 64 → 128 bits
    div.s      Division signée 64 / 64 → 64 bits
    test_*     Des tests sans rapport avec le rendu
    Makefile   Compile tout (au format binaire) sans se casser la tête

(Les programmes en .ps utilisent une syntaxe plus libre qui laisse l'assembleur
 se débrouiller pour déterminer quel opcode il doit utiliser.)

On a (beaucoup) modifié l'assembleur et le simulateur ; voyez plus bas pour
leur utilisation. Comme l'ISA rebondit pas mal, il y a encore des
incompatibilités entre notre implémentation et votre norme (constants 1-bit non
signées, gestions des flags...). Un fichier remarques.txt est joint au rendu
avec quelques remarques sur le jeu d'instructions.


## Assembleur (/compiler)

`./myasm.py --help` affiche l'aide. La commande est de la forme :

	./myasm.py [-h] [--stdout | -o FILE] (-m | -t | -b) SOURCE

L'option --stdout envoie la sortie dans le terminal, tandis que -o écrit dans
un fichier. Les options -m, -t et -b spécifient le format de sortie :
mnémoniques, texte et binaire. L'émulateur rendu ici ne supporte que le format
binaire ; le format officiel est le texte.

Il y a une option supplémentaire, --huffman, qui recalcule l'arbre de Huffman
pour optimiser spécifiquement le programme assemblé et produit un fichier
opcode.txt avec le résultat de l'analyse.


## Émulateur (/emu)

Pour compiler l'émulateur, lancez `make` dans le dossier /emu. On a eu des
problèmes à la con, veuillez voir /doc/emu_troubleshooting.md si quelque chose
se passe mal à un moment (erreur de linkage, mauvais rendu à l'écran...).
L'émulateur est invoqué avec une commande de la forme :

	./emu [-r|-d] BINAIRE [options...]

Il lui faut un fichier binaire généré par le mode -b de l'assembleur. Les
fichiers texte seront acceptés mais pas compris ; fournir un fichier texte est
une erreur.

L'émulateur supporte actuellement deux modes d'exécution :
- Le mode -r lance rapidement l'exécution et affiche l'état du processeur
  lorsque le programme se termine. (Par défaut)
- Le mode -d lance le debugger graphique, qui fournit l'exécution pas-à-pas,
  un désassembleur, et de quoi surveiller les registres à tout instant.

D'autres options existent pour modifier la géométrie de la mémoire émulée ; ce
n'est pas très important. Les détails sont dans /doc/emu_memory_layout.md.

Le debugger tourne dans une interface ncurses. La taille minimale utilisable de
terminal est 96 × 32 ; je recommande à partir de 128 × 40, sinon des infos sont
cachées. Le debugger est contrôlé par une ligne de commande qui a une aide
intégrée ; vous pouvez vous y référer.
