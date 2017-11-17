;---
;	Drawing library
;	All of the colors in this module are in 16-bit format.
;---

main:
	leti	r1 0
	leti	r2 0
	leti	r3 0xf800
	call	plot

	jump	-13

;	plot()
;	Changes the color of a pixel. Returns a pointer to the pixel at
;	location (x, y) (provided it exists).
;
;	@args	x, y, color
;	@ret	Pointer to the cell at (x, y)
plot:
	; Bit offset in VRAM area
	shift	left r2 5
	add2	r1 r2
	shift	left r2 2
	add2	r1 r2
	shift	left r1 4

	; VRAM pointer
	leti	r0 0x2
	shift	left r0 20
	add2	r1 r0

	; Load it to memory, and perform the plot
	getctr	a0 r2
	setctr	a0 r1
	write	a0 16 r3
	setctr	a0 r2
	return
