; CPU management: functions to access registers and stack (kinda repetitive).

; cpu_getPC() -> r0 = PC
cpu_getPC:
	leti	r0 0x88080 ; PC
	getctr	a0 r1
	setctr	a0 r0
	readze	a0 16 r0
	setctr	a0 r1
	return

; cpu_setPC(r1 = new_pc)
cpu_setPC:
	leti	r0 0x88080 ; PC
	getctr	a0 r2
	setctr	a0 r0
	write	a0 16 r1
	setctr	a0 r2
	return

; cpu_getI() -> r0 = I
cpu_getI:
	leti	r0 0x88090 ; I
	getctr	a0 r1
	setctr	a0 r0
	readze	a0 16 r0
	setctr	a0 r1
	return

; cpu_setI(r1 = new_i)
cpu_setI:
	leti	r0 0x88090 ; I
	getctr	a0 r2
	setctr	a0 r0
	write	a0 16 r1
	setctr	a0 r2
	return

; cpu_opcode() -> r0 = next opcode and PC += 2
cpu_opcode:
	push	64 r7
	push	64 r6

	call	cpu_getPC
	let	r6 r0

	let	r1 r0
	call	mem_opcode

	add3i	r1 r6 2
	let	r6 r0
	call	cpu_setPC
	let	r0 r6

	pop	64 r6
	pop	64 r7
	return

; cpu_getReg(r1 = reg) -> r0 = v[r1]
cpu_getReg:
	leti	r0 0x88000 ; Register base
	shift	left r1 3
	add2	r0 r1
	getctr	a0 r1
	setctr	a0 r0
	readze	a0 8 r0
	setctr	a0 r1
	return

; cpu_setReg(r1 = reg, r2 = val)
cpu_setReg:
	leti	r0 0x88000 ; Register base
	shift	left r1 3
	add2	r0 r1
	getctr	a0 r1
	setctr	a0 r0
	write	a0 8 r2
	setctr	a0 r1
	return

; cpu_push(r1 = pc)
cpu_push:
	leti	r0 0x881a0 ; SP
	getctr	a0 r3
	setctr	a0 r0
	readze	a0 16 r2
	add2i	r2 1
	setctr	a0 r0
	write	a0 16 r2

	leti	r0 0x880a0 ; Stack base
	sub2i	r2 1
	shift	left r2 4
	add2	r0 r2
	setctr	a0 r0
	write	a0 16 r1

	setctr	a0 r3
	return

; cpu_pop() -> r0 = pc
cpu_pop:
	leti	r0 0x881a0 ; SP
	getctr	a0 r3
	setctr	a0 r0
	readze	a0 16 r2
	sub2i	r2 1
	setctr	a0 r0
	write	a0 16 r2

	leti	r0 0x880a0 ; Stack base
	shift	left r2 4
	add2	r0 r2
	setctr	a0 r0
	readze	a0 16 r0

	setctr	a0 r3
	return
