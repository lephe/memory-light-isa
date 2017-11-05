; WARNING: THIS PROGRAM HAS *NOT* BEEN TESTED!

; Initialization (program input)
	leti	r0 46194
	leti	r1 135

; Main program (r2 = r0 / r1)
	leti	r2 0
	shift	left r1 15
	; The following takes only 24 bits, while `leti r3 0x8000` takes 42
	leti	r3 1
	shift	left r3 15

nonzero:
	cmp	r0 r1
	jumpif	slt 0 ; to be replaced with next
	add2	r2 r3
	sub2	r0 r1
next:
	shift	right r3 1
	shift	right r1 1
	jumpif	nz 0 ; to be replaced with nonzero

; Halt program (the emulator will detect this and avoid looping forever)
	jump	-13
