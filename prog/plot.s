
;  WIP

plot:
	; r0 : color
	; point (r1, r2).
	; I suppose the programmeur won't do shitty things.

	shift left r1 5
	let r3 r1
	shift left r3 2
	add r1 r3
	add r1 r2

	getctr	sp r2
	add r2 r1
	setctr	a0 r2

	write	a0 16 r0


