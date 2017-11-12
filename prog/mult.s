;-----------------------------------------------------------------------------;
;  Unsigned 64 * 64 -> 64 multiplication                                      ;
;-----------------------------------------------------------------------------;

; Initialization (program input)
	leti	r0 874
	leti	r1 65
	; r0 * r1 = 0xddea

; Main program (r2 = r0 * r1)
	leti	r2 0

; This loop contains 69 bits.
nonzero:
	shift	right r0 1
	jumpif	nc next
	add2	r2 r1
next:
	shift	left r1 1
	cmpi	r0 0
	jumpif	nz nonzero

; Halt program (the emulator will detect this and avoid looping forever)
	jump	-13
