; Testing constants and addresses of all sizes

; ALU constants
	leti	r0 1
	leti	r0 -1

	leti	r0 127
	leti	r0 -128

	leti	r0 255
	leti	r0 800

	leti	r0 10000
	leti	r0 -10000

	leti	r0 32767
	leti	r0 -32768

	leti	r0 0x7fffffff
	leti	r0 -0x80000000

	leti	r0 0x7fffffffffffffff
	leti	r0 -0x8000000000000000

; Addresses
	call	1
	call	-1

	call	127
	call	-128

	call	255
	call	-256

	call	10000
	call	-10000

	call	32767
	call	-32768

	call	0x7fffffff
	call	-0x80000000

	call	0x7fffffffffffffff
	call	-0x8000000000000000
