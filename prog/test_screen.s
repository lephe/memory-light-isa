; Test: plotting some pixels on the screen

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



	push	16 r3
	leti	r1 1
	leti	r2 4
	leti	r3 65
	call	putc

	call	_str_lea
	leti	r3 0xdefa
	push	16 r3
	leti	r1 1
	leti	r2 12
	let	r3 r0
	call	puts

	jump	-13

	; "Hello, World!"
_str_lea:
	getctr	pc r0
	add2i	r0 24
	return
_str:
	.const 112 #0100100001100101011011000110110001101111001011000010000001010111011011110111001001101100011001000010000100000000

	.include lib_draw.s
	.include lib_font.s
