;---
;	Wrappers for the drawing and font libraries
;	Include this file *in place of* the draw and font libraries with:
;	  .include wrapper.s
;---

	.include lib_draw.s
	.include lib_font.s

_clear_screen:
	let	r1 r0
	call	clear_screen
	return

_plot:
	let	r3 r0
	call	plot
	return

_draw:
	push	16 r0
	push	16 r4
	call	draw
	return

_fill:
	push	16 r0
	push	16 r4
	call	fill
	return

_putc:
	push	16 r0
	call	putc
	return

_puts:
	push	16 r0
	call	puts
	return
