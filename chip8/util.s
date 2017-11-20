; Various utility functions

; mult(r1 = x, r2 = y) -> r0 = x * y
mult:
	leti	r0 0

_mult_nonzero:
	shift	right r1 1
	jumpif	nc _mult_next
	add2	r0 r2
_mult_next:
	shift	left r2 1
	cmpi	r1 0
	jumpif	nz _mult_nonzero

	return

; clear_screen()
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
