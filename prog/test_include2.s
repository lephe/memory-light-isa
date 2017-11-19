jump main

.include test_include

main:
	let r1 1000
	let r2 1000
	call adder

	let r4 2000

halt:
	jump halt
