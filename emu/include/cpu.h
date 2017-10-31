//---
//	emu:cpu - emulate the CPU architecture
//
//	This module provides an interface for running binary code assembled for
//	the fictional architecture.
//---

#ifndef	CPU_H
#define	CPU_H

#include <defs.h>
#include <memory.h>

/* Some names for the memory pointers */
#define	PC	0
#define	SP	1
#define	A0	2
#define	A1	3

/*
	cpu_t structure
	Holds the values of all the registers and pointers of the CPU. The
	memory object associated with the CPU is used for code and data
	accesses. All pointer values as passed by the CPU to the memory, which
	does not have a copy of them.
*/
typedef struct
{
	memory_t *mem;		/* This one may be shared between CPUs */
	uint64_t r[8];		/* Remember to cast when required! */

	uint z	:1;		/* Zero		ie. x == y */
	uint c	:1;		/* Carry	ie. (uint)x < (uint)y */
	uint n	:1;		/* Negative	ie. (int)x < (int) y */

	uint64_t ptr[4];	/* There are no pointers among r0..r7 */
} cpu_t;

/*
	cpu_new() -- create a cpu and give it a memory
	The CPU does not retain ownership of the memory object. The memory
	object must be freed by the caller after the CPU is destroyed.

	@arg	mem	Memory to associate the CPU with
	@returns	New CPU object. Throws a fatal error on alloc failure.
*/
cpu_t *cpu_new(memory_t *mem);

/*
	cpu_destroy() -- destroy a cpu_t object (not its memory)

	@arg	cpu	CPU object allocated with cpu_new() to destroy
*/
void cpu_destroy(cpu_t *cpu);

/* TODO -
   Von Neumann cycles and stuff;
   Signals when interesting things happen, for debugger */

#endif	/* CPU_H */
