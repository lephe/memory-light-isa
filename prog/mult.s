; Initialization (program input)
	leti	r0 874
	leti	r1 65

; Main program (r2 = r0 * r1)
	leti	r2 0

; The loop size is trivially 69 bits because it's the size of the last jump.
nonzero:
	shift	right r0 1
	jumpif	nc 10
	add2	r2 r1
next:
	shift	left r1 1
	cmpi	r0 0
	jumpif	nz -69

; Halt program (the emulator will detect this and avoid looping forever)
	jump	-13
