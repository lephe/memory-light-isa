; Random-number generation

; srand(r1 = seed)
srand:
	leti	r0 557504
	getctr	a0 r2
	setctr	a0 r0
	write	a0 32 r1
	setctr	a0 r2
	return

; rand() -> r0 = random number
rand:
	push	64 r7
	push	64 r6

	leti	r0 557504
	getctr	a0 r6
	setctr	a0 r0

	readze	a0 32 r0
	leti	r1 1103515245
	call	mult
	add2i	r2 12345
	and2i	r2 0xffffffff

	leti	r0 557504
	setctr	a0 r0
	write	a0 32 r2

	setctr	a0 r6
	pop	64 r6
	pop	64 r7

	let	r0 r2
	return
