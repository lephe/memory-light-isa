clear_screen:
	; r0 containt color

	; get screen position
	getctr	sp r1
	setctr	a0 r1

	leti r0 0xffff

	leti r2 20480

pixel:
	write	a0 16 r0
	sub2i	r2 1
	jumpif	nz pixel


halt:
	jump -13