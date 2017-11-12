//---
//	emu:cpu - emulate the CPU architecture
//
//	This module provides an interface for running binary code assembled for
//	the fictional architecture.
//---

#ifndef	CPU_H
#define	CPU_H

#include <stdio.h>
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

	/* Flags for the debugger */
	uint h	:1;		/* Halt, detects loops of one instruction */
	uint m	:1;		/* Memory, indicates changes to memory */
	uint t	:1;		/* Counter, signals counter changes */

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

/*
	cpu_dump() -- print CPU state to a stream

	@arg	cpu	Which CPU to inspect
	@arg	stream	Stream to output to
*/
void cpu_dump(cpu_t *cpu, FILE *stream);

/*
	cpu_execute() -- read an execute an instruction
	This function changes the CPU state according to the instruction
	located in the associated memory at address PC. It also updates the
	debugger flags to signal interesting events.

	@arg	cpu	CPU which executes the instruction
*/
void cpu_execute(cpu_t *cpu);

#endif	/* CPU_H */
