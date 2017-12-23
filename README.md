# ASM 2017 : Rendu 3 (Alain Delaët-Tixeuil et Sébastien Michelland)

Le rendu graphique proposé est un émulateur chip8 dont les sources sont dans
le dossier /chip8. Du code, les seules choses intéressantes sont l'émulation du
jeu d'instructions (dans /chip8/main.s) et l'organisation des informations dans
la mémoire de l'émulateur (voir /doc/chip8_memory.md).

Un script, /play.sh, se trouve à la racine du projet pour lancer quelques jeux.
Pour obtenir la description des jeux, lancez /play.sh --help ; pour lancer un
jeu, essayez /play.sh BRIX (par exemple).

Pour arrêter l'émulation, interrompez le programme (^C) dans le terminal. Comme
le problème de l'arrêt est indécidable, il ne le fera probablement pas tout
seul.

    add2 324696   add2i 341633    sub2 0        sub2i 161537     cmp 140921    cmpi 1629180     let 823713    leti 807787
   shift 471559  readze 1329892  readse 0         jump 173606  jumpif 1933171     or2 60809     or2i 0         and2 0
   and2i 89856    write 391101    call 815274  setctr 1743155  getctr 891339    push 517084  return 815273    add3 191
   add3i 131881    sub3 900      sub3i 0         and3 11839    and3i 145315     or3 0         or3i 0         xor3 95
   xor3i 60561     asr3 0        sleep 10870    (res) 0        (res) 0
