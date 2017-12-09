; Memory management: basic memory access (CHIP-8 is basic in all regards).

; mem_opcode(r1 = PC) -> r0 = 16-bit opcode taken from PC
mem_opcode:
	leti	r0 0x80000 ; Start of memory
	shift	left r1 3
	add2	r0 r1

	getctr	a0 r1
	setctr	a0 r0
	readze	a0 16 r0
	setctr	a0 r1
	return

; mem_dump(r1 = x) -> stores v0..vx at the location given by I
mem_dump:
	push	64 r7
	push	64 r4

	add3i	r4 r1 1

	call	cpu_getI
	add3i	r1 r0 r4
	shift	left r0 3
	add2i	r0 0x80000 ; Memory base

	leti	r3 0x88000 ; Register base

	getctr	a0 r2
	setctr	a0 r0
	getctr	a1 r0
	setctr	a1 r3

_mem_dump_one:
	readze	a1 8 r3
	write	a0 8 r3
	sub2i	r4 1
	jumpif	nz _mem_dump_one

	setctr	a0 r2
	setctr	a1 r0

	call	cpu_setI
	pop	64 r4
	pop	64 r7
	return

; mem_load(r1 = x) -> load v0..vx from the location given by I
mem_load:
	push	64 r7
	push	64 r4

	add3i	r4 r1 1

	call	cpu_getI
	add3i	r1 r0 r4
	shift	left r0 3
	add2i	r0 0x80000 ; Memory base

	leti	r3 0x88000 ; Register base

	getctr	a0 r2
	setctr	a0 r0
	getctr	a1 r0
	setctr	a1 r3

_mem_load_one:
	readze	a0 8 r3
	write	a1 8 r3
	sub2i	r4 1
	jumpif	nz _mem_load_one

	setctr	a0 r2
	setctr	a1 r0

	call	cpu_setI
	pop	64 r4
	pop	64 r7
	return

; mem_flags_dump(r1 = x) -> store v0..vx to flags register (HP48)
mem_flags_dump:
	leti	r0 0x88000 ; Register base
	getctr	a0 r2
	setctr	a0 r0
	leti	r0 0x88200 ; HP48 flags
	getctr	a1 r3
	setctr	a1 r0

	add2i	r1 1

_mem_flags_dump_one:
	readze	a0 8 r0
	write	a1 8 r0
	sub2i	r1 1
	jumpif	nz _mem_flags_dump_one

	setctr	a0 r2
	setctr	a0 r3
	return

; mem_flags_load(r1 = x) -> load v0..vx from flags register (HP48)
mem_flags_load:
	leti	r0 0x88000 ; Register base
	getctr	a0 r2
	setctr	a0 r0
	leti	r0 0x88200 ; HP48 flags
	getctr	a1 r3
	setctr	a1 r0

	add2i	r1 1

_mem_flags_load_one:
	readze	a1 8 r0
	write	a0 8 r0
	sub2i	r1 1
	jumpif	nz _mem_flags_load_one

	setctr	a0 r2
	setctr	a0 r3
	return
