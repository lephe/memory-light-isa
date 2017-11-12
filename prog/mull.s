;-----------------------------------------------------------------------------;
;  Signed 64 * 64 -> 128 multiplication                                       ;
;-----------------------------------------------------------------------------;

; Initialization (program input)
	leti	r0 -0x374bc563deb482
	leti	r1 0x97b6af21f376
	; r0 * r1 = 0xffffffdf'3ad8dfa1'591295b1'6f3f6614 (negative)

; Main program ([r3r2] = r0 * [r6r1])
	leti	r2 0
	leti	r3 0
	leti	r6 0

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

; Calculate the product!
; The size of this loop is 149 bits.
loop:
	shift	right r0 1
	jumpif	nc next

	; Add with carry [r6r1] to [r3r2]
	add2	r3 r6
	add2	r2 r1
	jumpif	nc next
	add2i	r3 1

next:
	; Shift [r6r1] left 1 place
	shift	left r6 1
	shift 	left r1 1
	jumpif	nc nc1
	or2i	r6 1
nc1:
	cmpi	r0 0
	jumpif	nz loop

; Now swap signs if it was required
	shift	left r4 1
	jumpif	nc halt

	; Perform susbtraction with carry
	sub3	r2 r5 r2
	jumpif	nc nc2
	add2i	r3 1
nc2:
	sub3	r3 r5 r3

; Halt program (the emulator will detect this and avoid looping forever)
halt:
	jump	-13
