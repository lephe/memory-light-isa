; Main program

main:
	; PC = 0x200
	leti	r1 0x200
	call	cpu_setPC

	; Intialize RNG
	leti	r1 1
	call	srand

_von_neumann:
	; Fetch an instruction code, and PC += 2
	call	cpu_opcode

	; Execute this opcode. This is more or less a gigantic switch/case with
	; a lot of poorly-written instruction decoding
	let	r4 r0

_0000:
	; Halt program (?)
	cmpi	r4 0x0000
	jumpif	eq _main_end

_00e0:
	; Clear screen
	cmpi	r4 0x00e0
	jumpif	neq _00ee
	call	clear_screen
	jump	_von_neumann

_00ee:
	; Return
	cmpi	r4 0x00ee
	jumpif	neq _0nnn
	call	cpu_pop
	let	r1 r0
	call	cpu_setPC
	jump	_von_neumann

_0nnn:
	; Ignore RCA 1082 programs
	cmpi	r4 0x1000
	jumpif	lt _von_neumann

_1nnn:
	; Goto
	cmpi	r4 0x2000
	jumpif	ge _2nnn
	and3i	r1 r4 0xfff
	call	cpu_setPC
	jump	_von_neumann

_2nnn:
	; Call
	cmpi	r4 0x3000
	jumpif	ge _3xnn
	call	cpu_getPC
	let	r1 r0
	call	cpu_push
	and3	r1 r4 0xfff
	call	cpu_setPC
	jump	_von_neumann

_3xnn:
	; Snif vx = nn
	cmpi	r4 0x4000
	jumpif	ge _4xnn

	let	r1 r4
	shift	right r1 8
	and2i	r1 0xf
	call	cpu_getReg

	and3i	r1 r4 0xff
	cmp	r0 r1
	jumpif	neq _von_neumann

	call	cpu_getPC
	add3i	r1 r0 2
	call	cpu_setPC
	jump	_von_neumann

_4xnn:
	; Snif vx != nn
	cmpi	r4 0x5000
	jumpif	ge _5xy0

	let	r1 r4
	shift	right r1 8
	and2i	r1 0xf
	call	cpu_getReg

	and3i	r1 r4 0xff
	cmp	r0 r1
	jumpif	eq _von_neumann

	call	cpu_getPC
	add3i	r1 r0 2
	call	cpu_setPC
	jump	_von_neumann

_5xy0:
	; Snif vx == vy
	cmpi	r4 0x6000
	jumpif	ge _6xnn

	let	r1 r4
	shift	right r1 8
	and2i	r1 0xf
	call	cpu_getReg
	let	r5 r0

	and3i	r1 r4 0xf0
	shift	right r1 4
	call	cpu_getReg
	cmp	r5 r0
	jumpif	neq _von_neumann

	call	cpu_getPC
	add3i	r1 r0 2
	call	cpu_setPC
	jump	_von_neumann

_6xnn:
	; vx = nn
	cmpi	r4 0x7000
	jumpif	ge _7xnn
	let	r1 r4
	shift	right r1 8
	and2i	r1 0xf
	and3i	r2 r4 0xff
	call	cpu_setReg
	jump	_von_neumann

_7xnn:
	; vx += nn (no carry)
	cmpi	r4 0x8000
	jumpif	ge _8xyn_entry

	let	r1 r4
	shift	right r1 8
	and2i	r1 0xf
	let	r5 r1
	call	cpu_getReg

	and3i	r2 r4 0xff
	add2	r2 r0
	let	r1 r5
	call	cpu_setReg
	jump	_von_neumann

_8xyn_entry:
	cmpi	r4 0x9000
	jumpif	ge _9xy0

	and3i	r6 r4 0xf
	shift	right r4 4
	and3i	r5 r4 0xf
	shift	right r4 4
	and2i	r4 0xf

_8xy0:
	; vx = vy
	cmpi	r6 0
	jumpif	gt _8xy1
	let	r1 r5
	call	cpu_getReg
	let	r1 r4
	let	r2 r0
	call	cpu_setReg
	jump	_von_neumann

_8xy1:
	; vx |= vy
	cmpi	r6 1
	jumpif	gt _8xy2
	let	r1 r5
	call	cpu_getReg
	let	r5 r0
	let	r1 r4
	call	cpu_getReg
	let	r1 r4
	or3	r2 r0 r5
	call	cpu_setReg
	jump	_von_neumann

_8xy2:
	; vx &= vy
	cmpi	r6 2
	jumpif	gt _8xy3
	let	r1 r5
	call	cpu_getReg
	let	r5 r0
	let	r1 r4
	call	cpu_getReg
	let	r1 r4
	and3	r2 r0 r5
	call	cpu_setReg
	jump	_von_neumann

_8xy3:
	; vx |= vy
	cmpi	r6 3
	jumpif	gt _8xy4
	let	r1 r5
	call	cpu_getReg
	let	r5 r0
	let	r1 r4
	call	cpu_getReg
	let	r1 r4
	xor3	r2 r0 r5
	call	cpu_setReg
	jump	_von_neumann

_8xy4:
	; vx += vy
	cmpi	r6 4
	jumpif	gt _8xy5
	let	r1 r5
	call	cpu_getReg
	let	r5 r0
	let	r1 r4
	call	cpu_getReg
	let	r1 r4
	add2	r5 r0
	let	r2 r5
	call	cpu_setReg

	; Calculate carry
	shift	right r5 8
	leti	r1 15
	let	r2 r5
	call	cpu_setReg

_8xy5:
	; vx -= vy
	cmpi	r6 5
	jumpif	gt _8xy6
	let	r1 r5
	call	cpu_getReg
	let	r5 r0
	let	r1 r4
	call	cpu_getReg
	let	r1 r4
	sub3	r5 r0 r5
	let	r2 r5
	call	cpu_setReg

	; Calculate borrow
	shift	right r5 8
	and3i	r2 r5 1
	leti	r1 15
	call	cpu_setReg

_8xy6:
	; vx = vy = vy >> 1
	cmpi	r6 6
	jumpif	gt _8xy7
	let	r1 r5
	call	cpu_getReg
	let	r6 r0

	; Calculate carry
	and3i	r2 r6 1
	leti	r1 15
	call	cpu_setReg

	shift	right r6 1
	leti	r1 r4
	leti	r2 r6
	call	cpu_setReg

	let	r1 r5
	leti	r2 r6
	call	cpu_setReg
	jump	_von_neumann

_8xy7:
	; vx = vy - vx
	cmpi	r6 7
	jumpif	gt _8xye
	let	r1 r5
	call	cpu_getReg
	let	r5 r0
	let	r1 r4
	call	cpu_getReg
	let	r1 r4
	sub2	r5 r0
	let	r2 r5
	call	cpu_setReg

	; Calculate borrow
	shift	right r5 8
	and3i	r2 r5 1
	leti	r1 15
	call	cpu_setReg

_8xye:
	; vx = vy = vy << 1
	cmpi	r6 0xe
	jumpif	neq _8nnn

	let	r1 r5
	call	cpu_getReg
	let	r6 r0

	; Calculate carry
	let	r2 r6
	shift	right r2 8
	and2i	r2 1
	leti	r1 15
	call	cpu_setReg

	shift	left r6 1
	leti	r1 r4
	leti	r2 r6
	call	cpu_setReg

	let	r1 r5
	leti	r2 r6
	call	cpu_setReg
	jump	_von_neumann

_8nnn:
	; Invalid commands from the 8nnn range
	jump	_von_neumann

_9xy0:
	; Snif vx != vy
	cmpi	r4 0xa000
	jumpif	ge _annn

	let	r1 r4
	shift	right r1 8
	and2i	r1 0xf
	call	cpu_getReg
	let	r5 r0

	and3i	r1 r4 0xf0
	shift	right r1 4
	call	cpu_getReg
	cmp	r5 r0
	jumpif	eq _von_neumann

	call	cpu_getPC
	add3i	r1 r0 2
	call	cpu_setPC
	jump	_von_neumann

_annn:
	; I = nnn
	cmpi	r4 0xb000
	jumpif	ge _bnnn

	and3i	r1 r4 0xfff
	call	cpu_setI
	jump	_von_neumann

_bnnn:
	; PC = v0 + nnn
	cmpi	r4 0xc000
	jumpif	ge _cxnn

	leti	r1 0
	call	cpu_getReg
	and3i	r1 r4 0xfff
	add2	r1 r0
	call	cpu_setPC
	jump	_von_neumann

_cxnn:


	; Not a very thrilling loop
	jump	_von_neumann

_main_end:
	jump	-13

	.include cpu.s
	.include mem.s
	.include rand.s
	.include util.s
