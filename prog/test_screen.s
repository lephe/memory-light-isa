;-----------------------------------------------------------------------------;
;  Graphical rendering using memory-mapped video RAM                          ;
;-----------------------------------------------------------------------------;

	; Specify an "entry point"
	jump	main

	; This file exposes the clear_screen, plot, fill and draw functions
	.include lib_draw.s
	; This file exposes the putc and puts function
	.include lib_font.s

main:

; Clear the screen

	leti	r1 0x2105
	call	clear_screen

; Plot a line of pixels

	leti	r4 1

_plot_one:
	leti	r1 r4
	leti	r2 1
	leti	r3 0xdefa
	call	plot

	add2i	r4 2
	cmpi	r4 159
	jumpif	neq _plot_one

; Write "SM & AD", character by character, then "Hello, World!" using a string

	let	r5 r3

	; 'S'
	push	16 r5
	leti	r1 4
	leti	r2 7
	leti	r3 0x53
	call	putc
	; 'M'
	push	16 r5
	leti	r1 10
	leti	r2 7
	leti	r3 0x4d
	call	putc
	; '&'
	push	16 r5
	leti	r1 22
	leti	r2 7
	leti	r3 0x26
	call	putc
	; 'A'
	push	16 r5
	leti	r1 34
	leti	r2 7
	leti	r3 0x41
	call	putc
	; 'D'
	push	16 r5
	leti	r1 40
	leti	r2 7
	leti	r3 0x44
	call	putc

	; _str_lea retrieves the address of the string for us (see below)
	call	_str_lea
	leti	r3 0xdefa
	push	16 r3
	leti	r1 4
	leti	r2 18
	let	r3 r0
	call	puts

; Display a filled rectangle

	leti	r1 10
	leti	r2 98
	leti	r3 42
	leti	r0 0xdefa
	push	16 r0
	leti	r0 118
	push	16 r0
	call	fill

; Draw some lines in 8 directions

	; NEE
	leti	r1 40
	leti	r2 60
	leti	r3 60
	leti	r0 0xdefa
	push	16 r0
	leti	r0 70
	push	16 r0
	call	draw
	; NNE
	leti	r1 40
	leti	r2 60
	leti	r3 50
	leti	r0 0xdefa
	push	16 r0
	leti	r0 80
	push	16 r0
	call	draw
	; NNW
	leti	r1 40
	leti	r2 60
	leti	r3 30
	leti	r0 0xdefa
	push	16 r0
	leti	r0 80
	push	16 r0
	call	draw
	; NWW
	leti	r1 40
	leti	r2 60
	leti	r3 20
	leti	r0 0xdefa
	push	16 r0
	leti	r0 70
	push	16 r0
	call	draw
	; SWW
	leti	r1 40
	leti	r2 60
	leti	r3 20
	leti	r0 0xdefa
	push	16 r0
	leti	r0 50
	push	16 r0
	call	draw
	; SSW
	leti	r1 40
	leti	r2 60
	leti	r3 30
	leti	r0 0xdefa
	push	16 r0
	leti	r0 40
	push	16 r0
	call	draw
	; SSE
	leti	r1 40
	leti	r2 60
	leti	r3 50
	leti	r0 0xdefa
	push	16 r0
	leti	r0 40
	push	16 r0
	call	draw
	; SEE
	leti	r1 40
	leti	r2 60
	leti	r3 60
	leti	r0 0xdefa
	push	16 r0
	leti	r0 50
	push	16 r0
	call	draw

; Display a pentagon

	; These are the coordinates of the vertices. Each byte is a coordinate;
	; x values are in r4, y values are in r5.
	leti	r4 0x6d8a7f5c516d
	leti	r5 0x6e593737596e
	leti	r6 5

_line_one:
	; Get the coordinate of the current point
	and3i	r1 r4 0xff
	and3i	r2 r5 0xff
	shift	right r4 8
	shift	right r5 8

	; Draw a line from the current point to the next one
	and3i	r3 r4 0xff
	leti	r0 0xdefa
	push	16 r0
	and3i	r0 r5 0xff
	push	16 r0
	call	draw

	sub2i	r6 1
	jumpif	nz _line_one

; Stop the program (the emulator will detect this and avoid looping forever)

	jump	-13

; This is the string "Hello, World!", along with a "load" routine to retrieve
; its address

_str_lea:
	getctr	pc r0
	add2i	r0 24
	return
_str:
	; '#' means binary (the ISA does not specify)
	.const 112 #0100100001100101011011000110110001101111001011000010000001010111011011110111001001101100011001000010000100000000
