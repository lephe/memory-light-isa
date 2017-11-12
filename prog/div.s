; Note for teacher: as we have 64-bit registers, we perform 64/64 division.
; This program has no limitation on the size of the divider.

; Initialization (program input)
	leti	r0 -0x538ba20c467c034b
	leti	r1 0x6527dbd63
	; Note this is signed 64/64 division, r0 / r1 = 0xffffffff'f2c9133c

; Main program (r2 = r0 / r1)
	leti	r2 0
	leti	r3 1

; Manage signs (r4 < 0 iff we need to swap signs after dividing)
	xor3	r4 r0 r1
	; We need to have 0 in a register to calcule rx = 0 - rx
	leti	r5 0

	cmpi	r0 0
	jumpif	sgt r0_nonnegative
	sub3	r0 r5 r0
r0_nonnegative:
	cmpi	r1 0
	jumpif	sgt r1_nonnegative
	sub3	r1 r5 r1
r1_nonnegative:

; First, we need to shift r1 left so that its most significant bit reaches
; position 63. We do this in a naive way, which is costly.
shft:
	cmpi	r1 0
	jumpif	slt nonzero
	shift	left r3 1
	shift	left r1 1
	jump	shft

; Then we substract the divisor from the dividend when possible, and we shift
; right before trying again. This loop runs exactly 63 - lg(r1) times.
; Compiler announces the size of the loop is 80 bits.
nonzero:
	; If r1 >= r0, increase result and decrease r1
	cmp	r0 r1
	jumpif	slt next
	add2	r2 r3
	sub2	r0 r1
next:
	; In all cases, try with a smaller divisor in the next iteration
	shift	right r1 1
	shift	right r3 1
	jumpif	nz nonzero

; Now swap signs if it was required
	cmpi	r4 0
	; Jump if r4 >= 0
	jumpif	sgt halt
	jumpif	eq halt
	sub3	r2 r5 r2

; Halt program (the emulator will detect this and avoid looping forever)
halt:
	jump	-13
