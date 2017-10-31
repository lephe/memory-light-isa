; Initialization (program input)
	leti	r1 874
	leti	r2 65

; Main program (r3 = r1 * r2)
	leti	r3 0
nonzero:
	shift	right r1 1
	jumpif	nc 26
	add2	r3 r2
next:
	shift	left r2 1
	cmpi	r1 0
	jumpif	nz -53
