; WARNING: THIS PROGRAM IS NOT COMPATIBLE WITH THE ISA ANYMORE!
; Comprehensive instruction testing. Execute the program step-by-step and check
; the values of registers, PC, and flags when suggested.
; Untested instructions: readze, readse, write, push

; let and leti
	leti	r0 0
	leti	r1 1
	leti	r2 -1
	let	r3 r2
	leti	r4 0x4f3c
	leti	r5 -0x76528
	leti	r6 0x73926fc86c76b765
	let	r7 r4
	; CHECK that values correspond
	; If your registers are smaller than 64 bits, drop the higher bits

; add2, add2i, sub2, sub2i
	add2	r4 r2
	add2i	r4 15
	add2i	r4 0x87937cb4
	; CHECK r4 = 00000000'8793cbfe

	add2	r6 r6
	add2	r6 r6
	; CHECK zero clear, negative set, carry set

	sub2	r3 r3
	; CHECK zero set, negative clear, carry clear

	let	r5 0x100
	sub2i	r5 0x7f3c
	; CHECK r5 = ffffffff'ffff81c4, zero clear, negative set, carry set
	; If you use a carry instead of a borrow, you will know it here

; cmp, cmpi
	cmpi	r5 0
	; CHECK zero clear, negative set, carry clear
	; Note: this one is important, make sure it's carry *clear*

	leti	r2 15
	leti	r3 40
	cmp	r2 r3
	; CHECK zero clear, negative set, carry set

	leti	r2 -15
	cmp	r2 r3
	; CHECK zero clear, negative set, carry clear

	cmp	r3 40
	; CHECK zero set, negative clear, carry clear

	leti	r3 -40
	cmp	r2 r3
	; CHECK zero clear, negative clear, carry clear

; shift
	leti	r0 3
	shift	left r0 61
	; CHECK r0 = 60000000'00000000, zero clear, carry clear

	let	r1 r0
	shift	left r0 2
	; CHECK r0 = 80000000'00000000, zero clear, carry set

	shift	left r0 1
	; CHECK r0 = 0, zero set, carry set

	shift	left r0 1
	; CHECK r0 = 0, zero set, carry clear

	shift	right r1 60
	; CHECK r1 = 6, zero clear, carry clear

	shift	right r1 2
	; CHECK r1 = 1, zero clear, carry set

	shift	right r1 1
	; CHECK r1 = 0, zero set, carry set

	shift	right r1 1
	; CHECK r1 = 0, zero set, carry clear

; readze, readse
	; ...

; jump, jumpif
	jump	0
	; CHECK that it does nothing

	leti	r0 1
	jump	10
	add2	r0 r0
	; CHECK r0 = 1

	leti	r4 -80
	leti	r5 -78
	cmp	r4 r5
	; CHECK zero clear, negative set, carry set

	leti	r0 1
	leti	r1 1
	leti	r2 1
	leti	r3 1
	leti	r4 1
	leti	r5 1
	leti 	r6 1
	leti	r7 1

	jumpif	eq 9
	leti	r0 0
	jumpif	neq 9
	leti	r1 0
	jumpif	sgt 9
	leti	r2 0
	jumpif	slt 9
	leti	r3 0
	jumpif	gt 9
	leti	r4 0
	jumpif	ge 9
	leti	r5 0
	jumpif	lt 9
	leti	r6 0
	jumpif	le 9
	leti	r7 0
	; CHECK that r0..r7 = 0, 1, 0, 1, 0, 0, 1, 1

; or2, or2i, and2, and2i
	leti	r0 6
	leti	r1 12
	or2	r1 r0
	; CHECK r1 = 14, zero clear, negative clear

	or2i	r0 0xff
	; CHECK r0 = 0xff, zero clear, negative clear

	let	r4 -1
	or2	r0 r4
	; CHECK r0 = -1, zero clear, negative set

	leti	r0 6
	and2	r1 r0
	; CHECK r1 = 6, zero clear, negative clear

	and2i	r0 0xfa
	; CHECK r0 = 2, zero clear, negative clear

	and2i	r1 128
	; CHECK r1 = 0, zero set, negative clear

; call, return
; This section assumes that your program is loaded at address 0 (ie. the
; address of the first instruction is 0).

main:
	; If you know the address of the jump below, keep it in mind
	leti	r0 0
	call	0x4cc
	add2i	r0 100
	; CHECK r0 = 112

	jump	0x6c

func1:
	; Sub-function 1, calls sub 2 twice then returns
	; CHECK r7 = address of the jump above
	let	r5 r7
	add2i	r0 10
	call	0x528
	call	0x528
	let	r7 r5
	return

func2:
	; Sub-function 2
	add2i	r0 1
	return

; setctr, getctr
	getctr	a0 r7
	add2i	r7 4
	setctr	a0 r7
	; CHECK a0 = <your initial value> + 4

	leti	r0 24
	setctr	a1 r0
	; CHECK a1 = 24

	getctr	sp r4
	sub2i	r4 8
	setctr	sp r4
	; CHECK sp = <your initial value> - 8

; push, pop
	; ...

; add3, add3i, sub3, sub3i
	leti	r0 10
	leti	r1 -15
	add3	r2 r1 r0
	; CHECK r2 = ffffffff'fffffffb, zero clear, negative set, carry clear

	add3i	r2 r0 5
	; CHECK r2 = 15, zero clear, negative clear, carry clear

	sub3	r3 r2 r2
	; CHECK r3 = 0, zero set, negative clear, carry clear

	sub3i	r0 r1 20
	; CHECK r0 = ffffffff'ffffffdd, zero clear, negative set, carry clear

; or3, or3i, and3, and3i
	leti	r4 15
	leti	r5 70
	or3	r4 r4 r5
	; CHECK r4 = 79, zero clear, negative clear

	or3i	r4 r4 0xff
	; CHECK r4 = 0xff, zero clear, negative clear

	leti	r7 -1
	or3	r4 r4 r7
	; CHECK r4 = -1, zero clear, negative set

	leti	r6 12
	and3	r5 r4 r6
	; CHECK r5 = 12, zero clear, negative clear

	and3i	r5 r6 2
	; CHECK r5 = 0, zero set, negative clear

; xor3, xor3i
	leti	r7 12
	leti	r5 28
	xor3	r6 r5 r7
	; CHECK r6 = 16, zero clear

	xor3i	r6 r6 0x10
	; CHECK r6 = 0, zero set

; asr3
	leti	r5 15
	asr3	r7 r5 2
	; CHECK r7 = 3, zero clear

	asr3	r7 r7 2
	; CHECK r7 = 0, zero set

	leti	r5 -15
	asr3	r7 r5 1
	; CHECK r7 = -8, zero clear

	asr3	r7 r7 22
	; CHECK r7 = -1, zero clear

; You may need this to stop the program
	jump	-13
