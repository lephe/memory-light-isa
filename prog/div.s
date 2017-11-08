; WARNING: THIS PROGRAM HAS *NOT* BEEN TESTED!
; Note for teacher: as we have 64-bit registers, we perform 64/32 division

; Initialization (program input)
	leti	r0 0x3c745df3b983fcb5
	leti	r1 0x237dbd63
	; Note: r0 / r1 = 0x1b40ffcb6

; Main program (r2 = r0 / r1)
	leti	r2 0
	shift	left r1 31
	; The following takes only 24 bits, while `leti r3 ...` takes 74
	leti	r3 1
	shift	left r3 31

nonzero:
	cmp	r0 r1
	jumpif	slt next ; to be replaced with next
	add2	r2 r3
	sub2	r0 r1
next:
	shift	right r1 1
	shift	right r3 1
	jumpif	nz nonzero ; to be replaced with nonzero

; Halt program (the emulator will detect this and avoid looping forever)
	jump	-13
