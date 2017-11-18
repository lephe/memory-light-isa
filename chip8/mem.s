; Memory management: basic memory access (CHIP-8 is basic in all regards).

; mem_opcode(r1 = PC) -> r0 = 16-bit opcode taken from PC
mem_opcode:
	leti	r0 524288
	shift	left r1 3
	add2	r0 r1

	getctr	a0 r1
	setctr	a0 r0
	readze	a0 16 r0
	setctr	a0 r1
	return

; mem_dump() -> stores v0..vf at the location given by I
mem_dump:
	push	64 r7

	call	cpu_getI
	add3i	r1 r0 16
	shift	left r0 3
	add2i	r0 524288

	leti	r3 557056

	getctr	a0 r2
	setctr	a0 r0
	getctr	a1 r0
	setctr	a1 r3

	readze	a1 64 r3
	write	a0 64 r3
	readze	a1 64 r3
	write	a0 64 r3

	setctr	a0 r2
	setctr	a1 r0

	call	cpu_setI
	pop	64 r7

; mem_load() -> load v0..vf from the location given by I
mem_load:
	push	64 r7

	call	cpu_getI
	add3i	r1 r0 16
	shift	left r0 3
	add2i	r0 524288

	leti	r3 557056

	getctr	a0 r2
	setctr	a0 r0
	getctr	a1 r0
	setctr	a1 r3

	readze	a0 64 r3
	write	a1 64 r3
	readze	a0 64 r3
	write	a1 64 r3

	setctr	a0 r2
	setctr	a1 r0

	call	cpu_setI
	pop	64 r7

