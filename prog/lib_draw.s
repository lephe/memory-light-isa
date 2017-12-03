;---
;	Drawing library
;	All of the colors in this module are in 16-bit format.
;---

;	clear_screen()
;	Clears the whole screen in an efficient way.
;
;	@arg	color
clear_screen:
	; Load VRAM pointer and counter
	leti	r0 0x10000
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
	leti	r0 127
	sub3	r2 r0 r2
	shift	left r2 5
	add2	r1 r2
	shift	left r2 2
	add2	r1 r2
	shift	left r1 4

	; VRAM pointer
	leti	r0 0x10000
	add2	r1 r0

	; Load it to memory, and perform the plot
	getctr	a0 r2
	setctr	a0 r1
	write	a0 16 r3
	setctr	a0 r2
	return

;	draw()
;	Draws a line between (x1, y1) and (x2, y2) (both included). This
;	functions tries to circumvent the limitations imposed by the stack and
;	the calling conventions to efficiently manipulate a lot of variables.
;
;	@args	x1, y1, x2
;	@stack	y2(16), color(16)
draw:
	; Tricky swapping: get y2 and color in a single pop, then push r4
	; through r7, eventually push back color at the top and keep only y2.
	pop	32 r0
	push	64 r7
	push	64 r6
	push	64 r5
	push	64 r4
	push	16 r0
	shift	right r0 16
	let	r4 r0

	; Turn x2 into dx and y2 into dy. We don't want to change the
	; coordinate system since plot does it for us
	sub2	r3 r1
	sub2	r4 r2

	; Here is the state of things at this point:
	;   Registers  (free) x y dx dy (free) (free) (free)
	;   Stack      color(16) r4(64) r5(64) r6(64) r7(64)

	; Plot the first pixel
	pop	16 r0
	push	64 r3
	push	64 r2
	push	64 r1
	push	16 r0
	let	r3 r0
	call	plot
	pop	16 r0
	pop	64 r1
	pop	64 r2
	pop	64 r3
	push	16 r0

	; Distinguish vertical lines (one pixel per y) from horizontal lines
	; (one pixel per x). We need to compare abs(dx) and abs(dy), but
	; calculating both would be too long. So I use the following trick:
	;   abs(dx) > abs(dy)  <=>  sgn(dx + dy) = sgn(dx - dy)
	; which avoids branches.
	leti	r0 0
	add3	r5 r3 r4
	sub3	r6 r3 r4
	xor3	r5 r5 r6
	shift	left r5 1
	jumpif	c _draw_vert

	; Here is the storage used in the rest of the function. For the
	; vertical section, r6 is sx instead of sy (but it works the same way)
	;   Registers  (calculations) x y dx dy cumul sy i
	;   Stack      color(16) [things...] r4(64) r5(64) r6(64) r7(64)

; First situation: the line is horizontal
_draw_horiz:
	; Switch endpoints to ensure dx >= 0, saving a variable
	cmpi	r3 -1
	jumpif	sgt _draw_horiz_init
	add2	r1 r3
	add2	r2 r4
	sub3	r3 r0 r3
	sub3	r4 r0 r4

	_draw_horiz_init:
	; Calculate sy (in terms of +1 / -1, no 0 case is required) into r6
	let	r6 r4
	asr3	r6 r6 63
	add2	r6 r6
	add2i	r6 1

	; Take absolute value of dy
	cmpi	r4 -1
	jumpif	sgt _draw_horiz_init2
	sub3	r4 r0 r4

	_draw_horiz_init2:
	let	r5 r3
	shift	right r5 1
	let	r7 r3

	_draw_horiz_loop:
	add2i	r1 1
	add2	r5 r4
	cmp	r3 r5
	jumpif	ge _draw_horiz_plot
	sub2	r5 r3
	add2	r2 r6

	_draw_horiz_plot:
	; Save pretty much everything, perform the plot, and get the values
	; back. Having r0 as a buffer is very useful at this point
	pop	16 r0
	push	64 r7
	push	64 r3
	push	64 r2
	push	64 r1
	push	16 r0
	let	r3 r0
	call	plot
	pop	16 r0
	pop	64 r1
	pop	64 r2
	pop	64 r3
	pop	64 r7
	push	16 r0

	sub2i	r7 1
	jumpif	nz _draw_horiz_loop
	jump	_draw_end

; Second situation: the line is vertical
_draw_vert:
	; Make sure dy >= 0
	cmpi	r4 -1
	jumpif	sgt _draw_vert_init
	add2	r1 r3
	add2	r2 r4
	sub3	r3 r0 r3
	sub3	r4 r0 r4

	_draw_vert_init:
	; This time, we need sx rather than sy
	let	r6 r3
	asr3	r6 r6 63
	add2	r6 r6
	add2i	r6 1

	; Take absolute value of dx
	cmpi	r3 -1
	jumpif	sgt _draw_vert_init2
	sub3	r3 r0 r3

	_draw_vert_init2:
	let	r5 r4
	shift	right r5 1
	let	r7 r4

	_draw_vert_loop:
	add2i	r2 1
	add2	r5 r3
	cmp	r4 r5
	jumpif	ge _draw_vert_plot
	sub2	r5 r4
	add2	r1 r6

	_draw_vert_plot:
	; Same thing as before, there's a lot of things to save and restore
	pop	16 r0
	push	64 r7
	push	64 r3
	push	64 r2
	push	64 r1
	push	16 r0
	let	r3 r0
	call	plot
	pop	16 r0
	pop	64 r1
	pop	64 r2
	pop	64 r3
	pop	64 r7
	push	16 r0

	sub2i	r7 1
	jumpif	nz _draw_vert_loop

; End of function: restore and leave
_draw_end:
	pop	16 r0
	pop	64 r4
	pop	64 r5
	pop	64 r6
	pop	64 r7
	return

;	fill()
;	Fills a rectangle defined by its top-left corner (x1, y1) and its
;	bottom-right corner (x2, y2). Both endpoints are included.
;
;	@args	x1, y1, x2
;	@stack	y2(16), color(16)
fill:
	; Pop parameters and push callee-saved registers
	pop	32 r0
	push	64 r5
	push	64 r4
	let	r4 r0
	shift	right r4 16
	and3i	r5 r0 0xffff

	sub2	r3 r1
	sub2	r4 r2

	; Change coordinate system
	xor3i	r2 r2 127
	sub2	r2 r4

	let	r0 r1
	sub2i	r2 1
	shift	left r2 5
	add2	r0 r2
	shift	left r2 2
	add2	r0 r2
	shift	left r0 4
	add2i	r0 0x10000

	getctr	a0 r1
	setctr	a0 r0

	; State of CPU for this function:
	;   Registers  vram saved_a0 x_ctr w y_ctr color (r6) (r7)
	;   Stack      r5(65) r4(65)

_fill_row:
	add2i	r0 2560
	setctr	a0 r0
	let	r2 r3

_fill_pixel:
	write	a0 16 r5
	sub2i	r2 1
	jumpif	nz _fill_pixel

	sub2i	r4 1
	jumpif	nz _fill_row

_fill_end:
	setctr	a0 r1
	pop	64 r4
	pop	64 r5
	return
