leti r0 31 ;couleur
leti r1 100 ;x1
leti r2 78 ;y1
leti r3 50 ;x2
leti r4 100  ;y2

call draw
jump -13



jump drawend
draw:
;mettons les points dans le bon sens
cmp r1 r3
jumpif lt echange
let r5 r1
let r1 r3
let r3 r5
let r5 r2
let r2 r4
let r4 r5
echange:

;ceci prepare le premier point
sub3 r5 r3 r1 
shift left r5 1 ;r5 contient dx
sub3 r6 r4 r2
shift left r6 1 ;r6 contient dy
push 64 r1
push 64 r2
push 64 r3
push 64 r7
call plot
pop 64 r7
pop 64 r3
pop 64 r2
pop 64 r1


cmpi r6 0
jumpif slt negatif
cmp r5 r6
jumpif slt grandepente


;on met les donnees dans les bons registres pour ce cas
let r4 r6
sub3 r6 r3 r1

boucle:
cmp r1 r3
jumpif ge fin
add2i r1 1
sub2 r6 r4
cmpi r6 0
jumpif sgt chgmtpixel
add2i r2 1
add2 r6 r5
chgmtpixel:
push 64 r1
push 64 r2
push 64 r3
push 64 r7
call plot
pop 64 r7
pop 64 r3
pop 64 r2
pop 64 r1
jump boucle
fin:
return
drawend:

grandepente:
;mettons les bons registres
let r3 r5
let r5 r6
sub3 r6 r4 r2


bouclebis:
cmp r2 r4
jumpif ge finbis
add2i r2 1
sub2 r6 r3
cmpi r6 0
jumpif sgt chgmtpixelbis
add2i r1 1
add2 r6 r5
chgmtpixelbis:
push 64 r1
push 64 r2
push 64 r3
push 64 r7
call plot
pop 64 r7
pop 64 r3
pop 64 r2
pop 64 r1
jump bouclebis
finbis:
return


negatif:
push 64 r0
leti r0 0
sub2 r0 r6
cmp r5 r0
jumpif slt grandepentebis
pop 64 r0
;dans le huitieme octant
let r4 r6
sub3 r6 r3 r1

boucleter:
cmp r1 r3
jumpif ge finter
add2i r1 1
add2 r6 r4
cmpi r6 0
jumpif sgt chgmtpixelter
sub2i r2 1
add2 r6 r5
chgmtpixelter:
push 64 r1
push 64 r2
push 64 r3
push 64 r7
call plot
pop 64 r7
pop 64 r3
pop 64 r2
pop 64 r1
jump boucleter
finter:
return



grandepentebis:
pop 64 r0
;dans le septieme octant
let r3 r5
let r5 r6
sub3 r6 r4 r2
bouclequater:
cmp r4 r2
jumpif ge finquater
sub2i r2 1
add2 r6 r3
cmpi r6 0
jumpif slt chgmtpixelquater
add2i r1 1
add2 r6 r5
chgmtpixelquater:
push 64 r1
push 64 r2
push 64 r3
push 64 r7
call plot
pop 64 r7
pop 64 r3
pop 64 r2
pop 64 r1
jump bouclequater
finquater:
return

jump plotend
plot:
leti r3 0x100000
sub2i r2 127
shift left r2 9
sub2 r3 r2
shift left r2 2
sub2 r3 r2
shift left r1 4
add2 r3 r1
setctr a0 r3
write a0 16 r0
return
plotend:

