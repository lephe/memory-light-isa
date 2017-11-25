; getkey() -> pressed key
getkey:
	leti	r1 0x881b0 ; Key buffer
	getctr	a0 r2

_getkey_loop:
	setctr	a0 r1
	readze	a0 16 r3
	cmpi	r3 0
	jumpif	z _getkey_loop

	sub3i	r0 r1 1
	xor3	r1 r0 r1
	leti	r0 -1

_getkey_find:
	add2i	r0 1
	shift	right r1 1
	jumpif	nz _getkey_find

	setctr	a0 r2
	return
