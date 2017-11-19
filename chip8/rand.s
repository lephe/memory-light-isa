; Random-number generation with demonstrated use of static text memory

; srand(r1 = seed)
srand:
	push	64 r7
	call	_counter_lea
	getctr	a0 r2
	setctr	a0 r0
	write	a0 32 r1
	setctr	a0 r2
	pop	64 r7
	return

; rand() -> r0 = random number
rand:
	push	64 r7
	push	64 r6

	call	_counter_lea
	getctr	a0 r6
	setctr	a0 r0

	readze	a0 32 r0
	leti	r1 1103515245
	call	mult
	add2i	r2 12345
	and2i	r2 0xffffffff

	call	_counter_lea
	setctr	a0 r0
	write	a0 32 r2

	setctr	a0 r6
	pop	64 r6
	pop	64 r7

	let	r0 r2
	return

; Using static memory from the text segment (for a change)
_counter_lea:
	getctr	pc r0
	add2i	r0 35
	return
_counter:
	.const 32 0
