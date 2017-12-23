# ASM 2017 : Rendu 3 (Alain Delaët-Tixeuil et Sébastien Michelland)

Le rendu graphique proposé est un émulateur chip8 dont les sources sont dans
le dossier /chip8. Du code, les seules choses intéressantes sont l'émulation du
jeu d'instructions (dans /chip8/main.s) et l'organisation des informations dans
la mémoire de l'émulateur (voir /doc/chip8_memory.md).

Un script, /play.sh, se trouve à la racine du projet pour lancer quelques jeux.
Pour lancer un jeun, essayes `/play.sh BRIX` (par exemple). Une description des
jeux se trouve à la fin de ce fichier.

Pour arrêter l'émulation, interrompez le programme (^C) dans le terminal. Comme
le problème de l'arrêt est indécidable, il ne le fera probablement pas tout
seul.

    add2 324696   add2i 341633    sub2 0        sub2i 161537     cmp 140921    cmpi 1629180     let 823713    leti 807787
   shift 471559  readze 1329892  readse 0         jump 173606  jumpif 1933171     or2 60809     or2i 0         and2 0
   and2i 89856    write 391101    call 815274  setctr 1743155  getctr 891339    push 517084  return 815273    add3 191
   add3i 131881    sub3 900      sub3i 0         and3 11839    and3i 145315     or3 0         or3i 0         xor3 95
   xor3i 60561     asr3 0        sleep 10870    (res) 0        (res) 0

## Description des jeux

On espère que vous y trouverez votre bonheur.

- BLITZ
  Lâchez des bombes pour raser les buildings avant que votre avion, qui vole de
  plus en plus bas, ne s'écrase dedans. Utilisez Z pour lancer le jeu et pour
  lâcher les bombes.

- BRIX et ses variantes: BREAKOUT, VBRIX et WIPEOFF
  Contrôlez la palette avec A (gauche) et E (droite) (ou 1 (haut) et A (bas)
  dans le cas de VBRIX) pour détruire tous les objets sans perdre la balle.
  Utilisez Q pour quitter le menu de VBRIX et lancer le jeu.

- GUESS
  Choisissez un nombre entre 0 et 63 ; appuyez sur Z s'il est à l'écran,
  n'importe quelle autre touche sinon. Le programme devine le nombre après
  quelques étapes !

- HIDDEN
  16 cartes retournées, 8 paires de cartes identiques : vous devez reconstituer
  les paires. Utilisez A, E, 2 et S pour déplacer le curseur, Z pour révéler
  une carte.
  (NB. Ce jeu marche mal car le curseur se déplace trop vite.)

- MAZE
  Ce programme génère et affiche un labyrinthe aléatoire !

- MERLIN
  Reproduisez la séquence affichée sur les quatre cases en utilisant les
  touches A, Z, Q et S. La séquence s'allonge avec les niveaux !

- MISSILE
  Appuyez sur S pour tirer un missile. Vous devez détruire les 8 cibles en haut
  de l'écran, mais votre vaisseau accélère au fur et à mesure.

- PONG et PONG2
  L'incontournable à deux jours. Dirigez les palettes avec 1, A (gauche) et 4,
  R (droite).

- SCTEST
  Une ROM de test ; si elle n'affiche pas "OK", c'est que j'ai merdé.

- TETRIS
  Dirigez les blocs avec les touches Z (gauche) et E (droite). La rotation est
  sur la touche A, et l'accélération sur la touche Q.

- VERS
  Un snake à deux joueurs ; le premier se contrôle avec 1 (gauche), 2 (droite)
  et Q (haut), W (bas) ; le second avec C (gauche), V (droite), 4 (haut) et R
  (bas).
