;-----------------------------------------------------------------------------;
;  Signed 64 / 64 -> 64 division                                              ;
;-----------------------------------------------------------------------------;

; Initialization (program input)
	leti	r0 -0x538ba20c467c034b
	leti	r1 0x6527dbd63
	; Negative 64 / Positive 64 division, r0 / r1 = 0xffffffff'f2c9133c

; Main program (r2 = r0 / r1)
	leti	r2 0
	leti	r3 1

; Manage signs (r4 < 0 iff we need to swap signs after dividing)
	xor3	r4 r0 r1
	; We need to have 0 in a register to calculate rx = 0 - rx
	leti	r5 0

	cmpi	r0 0
	jumpif	sgt r0_positive
	sub3	r0 r5 r0
r0_positive:
	cmpi	r1 0
	jumpif	sgt r1_positive
	sub3	r1 r5 r1
r1_positive:

; First, we need to shift r1 left so that its most significant bit reaches
; position 63. This naive method is costly, however we cannot just compute
; lg(r1) and shift left by 64 - lg(r1) because we can only perform constant
; shifts. A logarithmic method is presented below.
shft:
	cmpi	r1 0
	jumpif	slt nonzero
	shift	left r3 1
	shift	left r1 1
	jump	shft

; Alternative method in C syntax (logarithmic complexity):
;	x = r1;
;	if(r1 & 0xffffffff == r1) x <<= 32;
;	else r1 >>= 32;
;	if(r1 & 0xffff == r1) x <<= 16;
;	else r1 >>= 16;
;	if(r1 & 0xff == r1) x <<= 8;
;	else r1 >>= 8;
;	...
;	if(r1 & 0x1 == r1) x <<= 1;
;	r1 = x;

; Then we remove the divisor from the dividend when possible, and we shift
; right before trying again. This loop runs exactly 63 - lg(r1) times.
; The size of this loop is 80 bits.
nonzero:
	; If r1 >= r0, increase result and decrease r1
	cmp	r0 r1
	jumpif	lt next
	add2	r2 r3
	sub2	r0 r1
next:
	; In all cases, try with a smaller divisor in the next iteration
	shift	right r1 1
	shift	right r3 1
	jumpif	nz nonzero

; Now swap signs if it was required
	shift	left r4 1
	jumpif	nc halt
	sub3	r2 r5 r2

; Halt program (the emulator will detect this and avoid looping forever)
halt:
	jump	-13
