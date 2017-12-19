; clear_screen()
clear_screen:
	; Load VRAM pointer and counter
	leti	r0 0x100000 ; VRAM base
	leti	r3 160
	getctr	a0 r2
	setctr	a0 r0

	let	r0 r1
	shift	left r0 16
	or2	r1 r0

	let	r0 r1
	shift	left r0 32
	or2	r1 r0

_clear_screen_loop:
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	sub2i	r3 1
	jumpif	nz _clear_screen_loop

	setctr	a0 r2
	return

; load_hexa() -> load character table at 0x80000
load_hexa:
	push	64 r7
	call	_load_hexa_lea
	pop	64 r7

	leti	r1 0x80000 ; Start of memory
	getctr	a0 r2
	getctr	a1 r3
	setctr	a0 r0
	setctr	a1 r1

	readze	a0 64 r0
	write	a1 64 r0
	readze	a0 64 r0
	write	a1 64 r0
	readze	a0 64 r0
	write	a1 64 r0
	readze	a0 64 r0
	write	a1 64 r0
	readze	a0 64 r0
	write	a1 64 r0
	readze	a0 64 r0
	write	a1 64 r0
	readze	a0 64 r0
	write	a1 64 r0
	readze	a0 64 r0
	write	a1 64 r0
	readze	a0 64 r0
	write	a1 64 r0
	readze	a0 64 r0
	write	a1 64 r0

	setctr	a0 r2
	setctr	a1 r3
	return

_load_hexa_lea:
	getctr	pc r0
	add2i	r0 24
	return
_load_hexa_data:
	.const 640 #1111000010010000100100001001000011110000001000000110000000100000001000000111000011110000000100001111000010000000111100001111000000010000111100000001000011110000100100001001000011110000000100000001000011110000100000001111000000010000111100001111000010000000111100001001000011110000111100000001000000100000010000000100000011110000100100001111000010010000111100001111000010010000111100000001000011110000111100001001000011110000100100001001000011100000100100001110000010010000111000001111000010000000100000001000000011110000111000001001000010010000100100001110000011110000100000001111000010000000111100001111000010000000111100001000000010000000

; draw(r1 = x, r2 = y, r3 = n) -> draw sprite from memory at I
draw:
	getctr	a0 r0
	push	64 r0
	getctr	a1 r0
	push	64 r0
	push	64 r4
	push	64 r5
	push	64 r7

	; Get destination pointer
	leti	r0 0x100000 ; VRAM base
	shift	left r2 10
	add2	r0 r2
	shift	left r1 4
	add2	r0 r1
	setctr	a1 r0

	; Number of pixels to write
	let	r4 r3

	; Get source pointer
	call	cpu_getI
	shift	left r0 3
	add2i	r0 0x80000 ; Memory base
	setctr	a0 r0

	leti	r1 8
	let	r5 0

	; Perform the drawing
_draw_loop:
	readze	a0 1 r0
	cmpi	r0 0
	jumpif	z _draw_clear

	; Before setting the pixel, heed for collisions (r5)
	getctr	a1 r2
	readze	a1 16 r3
	or2	r5 r3
	setctr	a1 r2

	; Flip the color of the pixel
	xor3i	r0 r3 0xffff
	write	a1 16 r0
	jump	_draw_next

_draw_clear:
	getctr	a1 r2
	add2i	r2 16
	setctr	a1 r2

_draw_next:
	sub2i	r1 1
	jumpif	nz _draw_loop

_draw_newline:
	getctr	a1 r2
	add2i	r2 896
	setctr	a1 r2

	leti	r1 8
	sub2i	r4 1
	jumpif	nz _draw_loop

_draw_end:
	; Collision results
	leti	r1 15
	leti	r2 1
	cmpi	r5 0
	jumpif	nz _draw_collision
	leti	r2 0
_draw_collision:
	call	cpu_setReg

	pop	64 r7
	pop	64 r5
	pop	64 r4
	pop	64 r0
	setctr	a1 r0
	pop	64 r0
	setctr	a0 r0
	return
