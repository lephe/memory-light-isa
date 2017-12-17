; Random-number generation

; srand(r1 = seed)
srand:
	leti	r0 0x881c0 ; RNG seed
	getctr	a0 r2
	setctr	a0 r0
	write	a0 32 r1
	setctr	a0 r2
	return

; rand() -> r0 = random number
rand:
	push	64 r7
	push	64 r6

	leti	r0 0x881c0 ; RNG seed
	getctr	a0 r6
	setctr	a0 r0

	readze	a0 32 r2
	leti	r1 1103515245
	call	mult
	add2i	r0 12345
	and2i	r0 0xffffffff

	leti	r2 0x881c0 ; RNG seed
	setctr	a0 r2
	write	a0 32 r0

	setctr	a0 r6
	pop	64 r6
	pop	64 r7

	return
