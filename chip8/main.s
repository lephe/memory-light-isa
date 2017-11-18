; Main program

	.include cpu.s
	.include mem.s

main:
	; PC = 0x200
	leti	r1 0x200
	call	cpu_setPC

_von_neumann:
	; Fetch an instruction code, and PC += 2
	call	cpu_opcode

	; Execute this opcode. This is more or less a gigantic switch/case with
	; a lot of poorly-written instruction decoding

_00e0:
	cmpi	r0 0x00e0
	jumpif	nz _00ee
	call	screen_clear
	jump	_von_neumann

_00ee:
	cmpi	r0 0x00ee
	jumpif	nz _0nnn
	call	cpu_return
	jump	_von_neumann

_0nnn:
	; Ignore RCA 1082 programs
	cmpi	r0 0x1000
	jumpif	slt _von_neumann

_1nnn:
	cmpi	r0 0x2000
	jumpif	sge _2nnn
	and3i	r1 r0 0xfff
	call	cpu_jump
	jump	_von_neumann

_2nnn:
	cmpi	r0 0x3000

	; Not a very thrilling loop
	jump	_von_neumann

