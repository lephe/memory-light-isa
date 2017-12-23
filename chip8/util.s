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

; bcd(r1 = x) -> r0 = BCD representation of x (24 bits)
bcd:
	push	64 r4
	push	64 r5
	push	64 r6
	push	64 r7

	leti	r4 0
	leti	r5 3
	leti	r6 r1

_bcd_one:
	; r0 = r6 / 10
	let	r1 r6
	leti	r2 0x199a
	call	mult
	shift	right r0 16
	let	r2 r0

	; r1 = r6 % 10
	shift	left r0 1
	let	r1 r0
	shift	left r0 2
	add2	r1 r0
	sub3	r1 r6 r1

	; Add a digit to r4
	shift	left r4 8
	or2	r4 r1

	let	r6 r2
	sub2i	r5 1
	jumpif	nz _bcd_one

	let	r0 r4
	pop	64 r7
	pop	64 r6
	pop	64 r5
	pop	64 r4
	return

; wait() -> wait for available execution cycles
wait:
	push	64 r6
	getctr	a1 r0
	push	64 r0

	leti	r6 0x88240

_wait_loop:
	setctr	a1 r6
	readze	a1 8 r0
	readze	a1 8 r1
	cmp	r0 r1
	jumpif	neq _wait_end

	sleep	0
	jump	_wait_loop

_wait_end:
	; Increase the number of instructions executed so far
	add2i	r1 1
	add2i	r6 8
	setctr	a1 r6
	write	a1 8 r1

	pop	64 r0
	setctr	a1 r0
	pop	64 r6
	return
