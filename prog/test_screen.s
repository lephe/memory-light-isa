; Test: plotting some pixels on the screen

main:
	; Load the video ram address to A0
	getctr	sp r0
	setctr	a0 r0

	; Colors: white, black, red, green, blue. Load them into memory
	leti	r0 0xffff
	write	a1 16 r0
	leti	r0 0x0000
	write	a1 16 r0
	leti	r0 0xf800
	write	a1 16 r0
	leti	r0 0x07e0
	write	a1 16 r0
	leti	r0 0x001f
	write	a1 16 r0

	getctr	a1 r0
	sub2i	r0 0x50
	setctr	a1 r0

	; Plot some lines of pixels
	leti	r1 5
color:
	readze	a1 16 r0
	leti	r5 640

pixel:
	write	a0 16 r0
	sub2i	r5 1
	jumpif	nz pixel

	sub2i	r1 1
	jumpif	nz color

	; Wait and see!
	jump	-13
