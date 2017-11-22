

  jump main

.include lib_draw.s

draw:
  ; draw a line between (r1, r2) and (r3, r4) with color r0

  let r6 r4
  sub r6 r2
  shift left r6 1


  let r5 r1
  sub r5 r3

  let r4 r5
  shift left r4 1


__begin_for_r1_leq_r3:
  cmp r1 r3
  jumpif z __end_for_r1_leq_r3
  ; core of the for loop

  add r1 1

  push 64 r1
  push 64 r2
  push 64 r3

  let r3 r0

  call plot ; draw pixel (r1, r2) with color r0

  pop 64 r3
  pop 64 r2
  pop 64 r1

__begin_if_r5_plus_r4_ge0:
  add r5 r4
  cmp r5 0
  jumpif slt __else

  ; Don't sub r5 with r4

  add r2 1

  jump __end_if
__else:
  sub r5 r4
__end_if:


  jump __begin_for_r1_leq_r3

__end_for_r1_leq_r3:
  
  return


;procédure tracerSegment(entier x1, entier y1, entier x2, entier y2) est
;  déclarer entier x, y, dx, dy ;
;  déclarer entier e ; // valeur d’erreur
;  déclarer entier e(1,0), e(0,1) ;  // incréments
;  dy ← y2 - y1 ;
;  dx ← x2 - x1 ;
;  y ← y1 ;  // rangée initiale
;  e ← -dx ;  // valeur d’erreur initiale
;  e(1,0) ←  dy × 2 ;
;  e(0,1) ← -dx × 2;
;  pour x variant de x1 jusqu’à x2 par incrément  de 1 faire
;    tracerPixel(x, y) ;
;    si (e ← e + e(1,0)) ≥ 0 alors  // erreur pour le pixel suivant de même rangée
;      y ← y + 1 ;  // choisir plutôt le pixel suivant dans la rangée supérieure
;      e ← e + e(0,1) ;  // ajuste l’erreur commise dans cette nouvelle rangée
;    fin si ;
;  fin pour ;
;fin procédure ;



main:
  let r0 0xFFFF
  let r1 10
  let r2 10
  let r3 50
  let r3 50

  call draw
