; To test the instructions, execute the program step-by-step and check the
; values of the simulator's registers, PC and flags when suggested.
; Untested instructions: readze, readse, write, push (memory-related)
; Note: small values are represented as decimal; 64-bit values are hexadecimal

; let and leti
	leti	r0 0
	leti	r1 1
	leti	r2 -1
	; CHECK that 1 is stored on 8 bits (10_00000001) and -1 is stored on 2
	; bits (0_1) in assembled code. (The ISA says signed 1-bit.)
	; If they're not, a correct simulator will set r1 = r2 = -1.

	let	r3 r2
	leti	r4 0x4f3c
	leti	r5 -0x76528
	leti	r6 0x73926fc86c76b765
	let	r7 r4
	; CHECK that all values correspond (if your registers are smaller than
	; 64 bits, drop the higher bits)

; add2, add2i, sub2, sub2i
	add2	r4 r2
	add2i	r4 15
	add2i	r4 0x87937cb4
	; CHECK r4 = 00000000'8793cbfe

	add2	r6 r6
	; CHECK zero clear, negative set, carry clear, overflow set

	add2	r6 r6
	; CHECK zero clear, negative set, carry set, overflow clear

	sub2	r3 r3
	; CHECK zero set, negative clear, carry *clear*, overflow clear
	; If you implemented a carry instead of a borrow, you will know it here

	leti	r5 0x100
	sub2i	r5 0x7f3c
	; CHECK r5 = ffffffff'ffff81c4, zero clear, negative set, carry set,
	; overflow clear

; cmp, cmpi
	cmpi	r5 0
	; CHECK zero clear, negative set, carry clear, overflow clear
	; Note: this one is important, make sure it's carry *clear* (you should
	; not catch the carry coming out of calculating -0)

	leti	r2 15
	leti	r3 40
	cmp	r2 r3
	; CHECK zero clear, negative set, carry set, overflow clear

	leti	r2 -15
	cmp	r2 r3
	; CHECK zero clear, negative set, carry clear, overflow clear

	cmpi	r3 40
	; CHECK zero set, negative clear, carry clear, overflow clear

	leti	r3 0x7fffffffffffffff
	cmp	r2 r3
	; CHECK zero clear, negative clear, carry clear, overflow set

; shift
	leti	r0 3
	shift	left r0 61
	; CHECK r0 = 60000000'00000000, zero clear, negative clear, carry clear

	let	r1 r0
	shift	left r0 2
	; CHECK r0 = 80000000'00000000, zero clear, negative set, carry set

	shift	left r0 1
	; CHECK r0 = 0, zero set, negative clear, carry set

	shift	left r0 1
	; CHECK r0 = 0, zero set, negative clear, carry clear

	shift	right r1 60
	; CHECK r1 = 6, zero clear, negative clear, carry clear

	shift	right r1 2
	; CHECK r1 = 1, zero clear, negative clear, carry set

	shift	right r1 1
	; CHECK r1 = 0, zero set, negative clear, carry set

	shift	right r1 1
	; CHECK r1 = 0, zero set, negative clear, carry clear

; readze, readse
	; Not tested (a memory view would be useful for these)

; jump, jumpif
	jump	0
	; CHECK that it does nothing (PC after this instruction should be PC
	; before this instruction + 13)

	leti	r0 1
	jump	10
	add2	r0 r0
	; CHECK r0 = 1

	leti	r4 -80
	leti	r5 -78
	cmp	r4 r5
	; CHECK zero clear, negative set, carry set, overflow clear

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
	jumpif	v 9
	leti	r7 0
	; CHECK that r0..r7 = 0, 1, 0, 1, 0, 0, 1, 0

; or2, or2i, and2, and2i
	leti	r0 6
	leti	r1 12
	or2	r1 r0
	; CHECK r1 = 14, zero clear, negative clear, carry clear

	or2i	r0 0xff
	; CHECK r0 = 0xff, zero clear, negative clear, carry clear

	leti	r4 -1
	or2	r0 r4
	; CHECK r0 = -1, zero clear, negative set, carry clear

	leti	r0 6
	and2	r1 r0
	; CHECK r1 = 6, zero clear, negative clear, carry clear

	and2i	r0 0xfa
	; CHECK r0 = 2, zero clear, negative clear, carry clear

	and2i	r1 128
	; CHECK r1 = 0, zero set, negative clear, carry clear

; call, return
; This section assumes that your program starts at address 0. Because I used no
; labels, your compiler must choose the minimal argument size for all
; constants, otherwise the call addresses will get messed up.
; You will need more "interactive" execution for the following tests. Pay
; extra attention to the values of PC and r7 at each step.

main:
	; If you can get the address of the add2i below, write it down
	leti	r0 0
	call	0x545
	add2i	r0 100
	; When you reach this point, check that r0 = 112

	jump	0x6c

func1:
	; Sub-function 1, calls sub-function 2 twice then returns
	; CHECK r7 = address of the add2i above
	let	r5 r7
	add2i	r0 10
	call	0x5a1
	call	0x5a1
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
	; Not tested (a memory view would be useful for these)

; add3, add3i, sub3, sub3i
	leti	r0 10
	leti	r1 -15
	add3	r2 r1 r0
	; CHECK r2 = ffffffff'fffffffb, zero clear, negative set, carry clear,
	; overflow clear

	add3i	r2 r0 5
	; CHECK r2 = 15, zero clear, negative clear, carry clear, overflow
	; clear

	leti	r2 -0x7fffffffffffffff
	leti	r3 2
	sub3	r3 r2 r3
	; CHECK r3 = -r2, zero clear, negative clear, carry clear, overflow set

	sub3i	r0 r1 20
	; CHECK r0 = ffffffff'ffffffdd, zero clear, negative set, carry clear,
	; overflow clear

; or3, or3i, and3, and3i
	leti	r4 15
	leti	r5 70
	or3	r4 r4 r5
	; CHECK r4 = 79, zero clear, negative clear, carry clear

	or3i	r4 r4 0xff
	; CHECK r4 = 0xff, zero clear, negative clear, carry clear

	leti	r7 -1
	or3	r4 r4 r7
	; CHECK r4 = -1, zero clear, negative set, carry clear

	leti	r6 12
	and3	r5 r4 r6
	; CHECK r5 = 12, zero clear, negative clear, carry clear

	and3i	r5 r6 2
	; CHECK r5 = 0, zero set, negative clear, carry clear

; xor3, xor3i
	leti	r7 12
	leti	r5 28
	xor3	r6 r5 r7
	; CHECK r6 = 16, zero clear, negative clear, carry clear

	xor3i	r6 r6 0x10
	; CHECK r6 = 0, zero set, negative clear, carry clear

; asr3
	leti	r5 15
	asr3	r7 r5 2
	; CHECK r7 = 3, zero clear, negative clear, carry set

	asr3	r7 r7 2
	; CHECK r7 = 0, zero set, negative clear, carry set

	leti	r5 -15
	asr3	r7 r5 1
	; CHECK r7 = -8, zero clear, negative set, carry set

	asr3	r7 r7 22
	; CHECK r7 = -1, zero clear, negative set, carry set

; You may need this to stop the program
	jump	-13
