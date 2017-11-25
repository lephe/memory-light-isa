; Main program

main:
	; PC = 0x200
	leti	r1 0x200
	call	cpu_setPC

	; Initialize RNG
	leti	r1 1
	call	srand

	; Clear the screen to black
	leti	r1 0x0000
	call	clear_screen

	; Load the character data in the beginning of the memory
	call	load_hexa

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
	jumpif	neq _von_neumann

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
	; vx = rand() & 0xnn
	cmpi	r4 0xd000
	jumpif	ge _dxyn
	call	rand
	and3	r2 r0 r4
	shift	left r4 8
	and3i	r1 r4 0xf
	call	cpu_setReg
	jump	_von_neumann

_dxyn:
	; Draw sprite at vx, vy with height n
	cmpi	r4 0xe000
	jumpif	ge _ennn
	and3i	r3 r4 0xf
	shift	right r4 4
	and3i	r2 r4 0xf
	shift	right r4 4
	and2i	r1 r4 0xf
	call	draw
	jump	_von_neumann

; Frow now on, we'll need to low nibble to distinguish between instructions

_ennn:
	and3i	r5 r4 0xff
	let	r0 r4
	shift	right r4 8
	and2i	r4 0xf
	cmpi	r0 0xf000
	jumpif	ge _fx07

_ex9e:
	; snif key(vx) is pressed
	cmpi	r5 0x9e
	jumpif	neq _exa1
	leti	r0 0x881b0 ; Key buffer
	add2	r4 r0
	getctr	a0 r2
	setctr	a0 r0
	readze	a0 1 r1
	setctr	a0 r2

	cmpi	r1 0
	jumpif	z _von_neumann
	call	cpu_getPC
	add2i	r1 r0 2
	call	cpu_setPC
	jump	_von_neumann

_exa1:
	; snif key(vx) is released
	cmpi	r5 0xa1
	jumpif	neq _von_neumann
	leti	r0 0x881b0 ; Key buffer
	add2	r4 r0
	getctr	a0 r2
	setctr	a0 r0
	readze	a0 1 r1
	setctr	a0 r2

	cmpi	r1 0
	jumpif	nz _von_neumann
	call	cpu_getPC
	add2i	r1 r0 2
	call	cpu_setPC
	jump	_von_neumann

_fx07:
	; vx = delay timer
	cmpi	r5 0x07
	jumpif	neq _fx0a
	leti	r0 0x881e0 ; Delay timer
	getctr	a0 r1
	readze	a0 8 r2
	setctr	a0 r1
	let	r1 r4
	call	cpu_setReg
	jump	_von_neumann

_fx0a:
	; vx = getkey()
	cmpi	r5 0x0a
	jumpif	neq _fx15
	call	getkey
	let	r1 r4
	let	r2 r0
	call	cpu_setReg
	jump	_von_neumann

_fx15:
	; delay timer = vx
	cmpi	r5 0x15
	jumpif	neq _fx18
	let	r1 r4
	call	cpu_getReg
	leti	r1 0x881e0 ; Delay timer
	getctr	a0 r2
	setctr	a0 r1
	write	a0 8 r0
	setctr	a0 r2
	jump	_von_neumann

_fx18:
	; sound timer = vx
	cmpi	r5 0x18
	jumpif	neq _fx1e
	let	r1 r4
	call	cpu_getReg
	leti	r1 0x881f0 ; Sound timer
	getctr	a0 r2
	setctr	a0 r1
	write	a0 8 r0
	setctr	a0 r2
	jump	_von_neumann

_fx1e:
	; I += vx
	cmpi	r5 0x1e
	jumpif	neq _fx29
	let	r1 r4
	call	cpu_getReg
	let	r4 r0
	call	cpu_getI
	add3	r1 r0 r4
	call	cpu_setI
	jump	_von_neumann

_fx29:
	; I = hexa sprite address (vf)
	cmpi	r5 0x29
	jumpif	neq _fx33
	leti	r1 15
	call	cpu_getReg
	shift	left r0 3
	let	r1 r0
	shift	left r0 2
	add2	r1 r0
	add2i	r1 0x80000 ; Start of memory
	call	cpu_setI
	jump	_von_neumann

_fx33:
	; Load BCD representation of vx into memory at I, I+1, I+2
	cmpi	r5 0x33
	jumpif	neq _fx55
	let	r1 r4
	call	cpu_getReg
	let	r1 r0
	call	bcd
	let	r4 r0
	call	cpu_getI
	shift	left r0 3
	add2i	r0 0x80000 ; Start of memory
	getctr	a0 r1
	setctr	a0 r0
	write	a0 8 r4
	shift	right r4 8
	write	a0 8 r4
	shift	right r4 8
	write	a0 8 r4
	setctr	a0 r1
	jump	_von_neumann

_fx55:
	; Save registers to memory at I
	cmpi	r5 0x55
	jumpif	neq _fx65
	call	mem_dump
	jump	_von_neumann

_fx65:
	; Load registers from memory at I
	cmpi	r5 0x65
	jumpif	neq _von_neumann
	call	mem_load
	jump	_von_neumann

_main_end:
	jump	-13

	.include cpu.s
	.include mem.s
	.include rand.s
	.include util.s
	.include draw.s
	.include keyboard.s
