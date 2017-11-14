#ifndef	UTIL_H
#define	UTIL_H

#include <stdint.h>
#include <defs.h>

/*
	sign_extend() -- sign-extend from a variable-width storage format
	Performs sign extension of the value x, which is stored in a signed
	n-bit format, into 64-bit signed format.

	@arg	x	Value to sign-extend
	@arg	n	Number of bits on which x is represented
	@returns	x in signed 64-bit format
*/
int64_t sign_extend(uint64_t x, uint n);

#endif	/* UTIL_H */
