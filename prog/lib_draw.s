;---
;	Drawing library
;	All of the colors in this module are in 16-bit format.
;---

main:
	leti	r1 0x2105
	call	clear_screen

	leti	r4 1

_plot_one:
	leti	r1 r4
	leti	r2 1
	leti	r3 0xdefa
	call	plot

	add2i	r4 2
	cmpi	r4 11
	jumpif	neq _plot_one

	jump	-13

;	clear_screen()
;	Clears the whole screen in an efficient way.
;
;	@arg	color
clear_screen:
	; Load VRAM pointer and counter
	leti	r0 0x100000
	leti	r3 640
	getctr	a0 r2
	setctr	a0 r0

	let	r0 r1
	shift	left r0 16
	or2	r1 r0

	let	r0 r1
	shift	left r0 32
	or2	r1 r0

_loop:
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	write	a0 64 r1
	sub2i	r3 1
	jumpif	nz _loop

	setctr	a0 r2
	return

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
	leti	r0 0x100000
	add2	r1 r0

	; Load it to memory, and perform the plot
	getctr	a0 r2
	setctr	a0 r1
	write	a0 16 r3
	setctr	a0 r2
	return
