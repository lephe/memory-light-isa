;---
;	Drawing library
;	All of the colors in this module are in 16-bit format.
;---

;	clear_screen()
;	Clears the whole screen in an efficient way.
;
;	@arg	color
clear_screen:
	; Load VRAM pointer and counter
	leti	r0 0x100000
	leti	r3 640
	getctr	a0 r2
	setctr	a0 r0

	let	r0 r1
	shift	left r0 16
	or2	r1 r0

	let	r0 r1
	shift	left r0 32
	or2	r1 r0

_loop:
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	sub2i	r3 1
	jumpif	nz _loop

	setctr	a0 r2
	return

;	plot()
;	Changes the color of a pixel. Returns a pointer to the pixel at
;	location (x, y) (provided it exists).
;
;	@args	x, y, color
;	@ret	Pointer to the cell at (x, y)
plot:
	; r1 = x
	; r2 = y
	; r3 = color
	; Bit offset in VRAM area
	leti	r0 127
	sub3	r2 r0 r2
	shift	left r2 5
	add2	r1 r2
	shift	left r2 2
	add2	r1 r2
	shift	left r1 4

	; VRAM pointer
	leti	r0 0x100000
	add2	r1 r0

	; Load it to memory, and perform the plot
	getctr	a0 r2
	setctr	a0 r1
	write	a0 16 r3
	setctr	a0 r2
	return

;	fill()
;	Change the color of a retangle (x, y) to - (x' y').
;
;   (0, 0) -----------------------------
;      |
;      |
;      |    (x, y ) ------------ (x', y)
;      |       |                     |
;      |       |                     |
;      |       |                     |
;      |       |                     |
;      |    (x, y') ------------ (x', y')
;      |
;      |
;	@args	x, y, x', y', color
;	@ret	Nothing
;   Verision préliminaire : j'utilise les compteurs comme je veux.
;   J'essayerais après de faire une version qui respecte les conventions
;   d'appels.

fill:
	; color -> r0
	; x -> r1
	; y -> r2
	; x' -> r3
	; y' -> r4

	sub r3 r1
	push 64 r7

	; felling like playing freecell...

	; r0 will be the increasing counter, starting at y finishing at y'.
	push 64 r2

	; let r1 be the 1D-coordonate of the pixel on the up left corner.
	shift	left r2 5
	add2	r1 r2
	shift	left r2 2
	add2	r1 r2
	shift	left r1 4
	add2i	r1 0x100000 ; position in the memory of the graphics.

	pop 64 r2

	leti r5 0
	;leti r7 160 ; for negation

	let r6 r4

	shift left r6 4 ; r6 <- 16 * r6

	; while r2 < r4 = y' :
__begin_while_y:
	cmp 	r2	r4
	jumpif	z	__end_while_y

	leti r5 0
	; while r5 < r2 = x' - x:
__begin_while_x:
	cmp r5 r2
	jumpif	z	__end_while_x

	write a0 16 r0

	add2i	r1 0x10
	add2i	r5 1
	jump __begin_while_x
__end_while_x:
	; delete r5
	leti	r5 160
	sub3	r3 r5 r3
	getctr	a0 r5
	add2	r5 r3
	setctr	a0 r5
	leti	r5 160
	sub3	r3 r5 r3




	add2i	r2	1
	jump	__begin_while_y
__end_while_y:

	pop 64 r7
	return