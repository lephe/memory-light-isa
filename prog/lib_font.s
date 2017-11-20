;---
;	Font library
;	No dependencies. 16-bit colors.
;---

;	putc()
;	Puts a character on the screen; (x, y) is the coordinate of the top-
;	left corner.
;
;	@args	x, y, c
;	@stack	color(16)
putc:
	; Get a pointer to the glyph data like a boss. I know that I should
	; save r1-r3, but I don't care that much (since it's font_lea)
	push	64 r7
	call	font_lea
	pop	64 r7

	; Add offset (35 * c)
	add2	r0 r3
	add2	r0 r3
	add2	r0 r3
	shift	left r3 5
	add2	r0 r3

	; Get a pointer to the appropriate video RAM location
	leti	r3 0x100000

	; Add 160 * y (* 16)
	shift	left r2 9
	add2	r3 r2
	shift	left r2 2
	add2	r3 r2

	; Add x (* 16)
	shift	left r1 4
	add2	r3 r1

	pop	16 r1
	getctr	a0 r2
	setctr	a0 r0
	getctr	a1 r0
	setctr	a1 r3

	; At this point:
	;   r0 = saved a1, r1 = color, r2 = saved a0
	;   a0 = source glyph, a1 = destination VRAM
	; And in the end of the routine:
	;   r3 = x-counter, r4 = glyph bits (sometimes a1), r5 = y-counter
	push	64 r4
	push	64 r5

	leti	r5 7
_putc_line:
	leti	r3 5

_putc_pixel:
	readze	a0 1 r4
	cmpi	r4 0
	jumpif	z _putc_clear
_putc_set:
	write	a1 16 r1
	jump	_putc_pixel_end
_putc_clear:
	getctr	a1 r4
	add2i	r4 16
	setctr	a1 r4
_putc_pixel_end:
	sub2i	r3 1
	jumpif	nz _putc_pixel

	leti	r3 155
	shift	left r3 4
	getctr	a1 r4
	add2i	r4 r3
	setctr	a1 r4

	sub2i	r5 1
	jumpif	nz _putc_line

	; Restore context and leave
	pop	64 r5
	pop	64 r4
	setctr	a0 r2
	setctr	a1 r0
	return

;	puts()
;	Writes a zero-terminated string on the screen by repeatedly calling
;	putc() for each of the characters.
;
;	@args	x, y, str
;	@stack	color(16)
puts:
	pop	16 r0

	push	64 r7
	push	64 r6
	push	64 r5
	push	64 r4

	let	r6 r0
	getctr	a0 r4
	push	64 r4
	setctr	a0 r3

	let	r4 r1
	let	r5 r2

_puts_one:
	readze	a0 8 r3
	cmpi	r3 0
	jumpif	z _puts_end

	let	r1 r4
	let	r2 r5
	add2i	r4 6
	push	16 r6
	call	putc
	jump	_puts_one

_puts_end:
	pop	64 r4
	setctr	a0 r4
	pop	64 r4
	pop	64 r5
	pop	64 r6
	pop	64 r7
	return

; Here be dragons (fonts)
; Encoding:
;   - Each character (5 * 7) takes 35 bytes
;   - Each glyph is stored from top to bottom, then from left to right
;   - All ASCII characters are present (thus 128 * 35 = 4480), in order
; Algorithm to draw a character (c, x0, y0, fg, bg):
;   y <- y0
;   Get a pointer to font + 35 * c
;   Repeat 7 times
;     x <- x0
;     Repeat 5 times
;       Fetch a bit
;       If it's 1, plot fg at (x, y), otherwise plot bg at (x, y)
;       x <- x + 1
;     y <- y + 1

font_lea:
	getctr	pc r0
	; Add the size of this routine
	add2i	r0 24
	return

font:
	.const 4480 #1111110001100011000110001100011111100000000100011001110001100001000000000000100001100011100110001000000000000000010111110010011111010000000000000100010101000100010101000100000000000010000000111110000000100000000000000100111100000111110001000000001100000001110001100011000110011110000000000011110100001110010000111100000000100000101111100010001000000000001000010010101001111110100000100000100010001000111110000011111000000100000100000101111100000111110000000000001000111010101001000010000000000000010000100101010111000100000000000000100010001111101000001000000001000001000111000001011111000101111001000101001110000010111110001011110100000100011101000111111100000111000010001000111010001111111000001110001000101001110100011111110000011100101000000011101000111111100000111000100010100000001100001000010001110010100000000000011000010000100011100010001010000000111010001100010111001010000000111010001100011000101110000000111010000100000111000100111000000000000011011001010010100100110100000001100100101110010010110110010000000111010001111111000110001011100000000100011101010110101011100010000000000001111101010010100101010011000000000000000000000000000000000000010000100001000010000100000000010001010010100000000000000000000000000010100101011111010101111101010010100010001111101000111000101111100010011000110010001000100010001001100011011001001010100010001010110010011010110000100010000000000000000000000000010001000100001000010000010000010010000010000010000100001000100010000000000100101010111001110101010010000000001000010011111001000010000000000000000000000000000011000010001000000000000000001111100000000000000000000000000000000000000000110001100000000000100010001000100010000000000111010001100111010111001100010111000100011000010000100001000010001110011101000100001000100010001000111111111100010001000001000001100010111000010001100101010010111110001000010111111000011110000010000110001011100011001000100001111010001100010111011111000010001000100001000010000100011101000110001011101000110001011100111010001100010111100001000100110000000011000110000000011000110000000000000110001100000000110000100010000001000100010001000001000001000001000000000001111100000111110000000000010000010000010000010001000100010000111010001000010001000100000000010001110100010000101101101011010101110011101000110001100011111110001100011111010001100011111010001100011111001110100011000010000100001000101110111101000110001100011000110001111101111110000100001111010000100001111111111100001000011110100001000010000011101000110000101111000110001011111000110001100011111110001100011000101110001000010000100001000010001110001110001000010000100001010010011001000110010101001100010100100101000110000100001000010000100001000011111100011101110101100011000110001100011000111001101011001110001100011000101110100011000110001100011000101110111101000110001100011111010000100000111010001100011000110001011100011111110100011000110001111101001010001011101000110000011100000110001011101111100100001000010000100001000010010001100011000110001100011000101110100011000110001100011000101010001001000110001101011010110101101010101010001100010101000100010101000110001100011000110001010100010000100001001111100001000100010001000100001111101110010000100001000010000100001110000001000001000001000001000001000000111000010000100001000010000100111000100010101000100000000000000000000000000000000000000000000000000111110110001000001000000000000000000000000000000000111000001011111000101111100001000011110100011000110001111100000000000011101000110000100010111000001000010111110001100011000101111000000000001110100011111110000011100001100100001000111000100001000010000000000000111110001011110000101110100001000010110110011000110001100010010000000011000010000100001000111000010000000011000010000101001001100010000100001001010100110001010010010110000100001000010000100001000111000000000001101010101101011010110101000000000010110110011000110001100010000000000011101000110001100010111000000000001111010001111101000010000000000000001111100010111100001000010000000000101101100110000100001000000000000000111110000011100000111110010001110001000010000100001001001100000000000100011000110001100110110100000000001000110001100010101000100000000000010001101011010110101010100000000000110010011000100011001001100000000001000101001001100010011000000000000011111000100010001000111110011000100001000100000100001000011000100001000010000100001000010000100011000010000100000100010000100011000000000000011011011000000000000000011111100011000110001100011000111111
