#ifndef	CPU_H
#define	CPU_H

#include <defs.h>
#include <memory.h>

/* Memory pointers */
#define	PC	0
#define	SP	1
#define	A0	2
#define	A1	3

typedef struct
{
	memory_t *mem;
	uint64_t r[8];

	uint z	:1;	/* Zero flag */
	uint c	:1;	/* Carry flag */
	uint n	:1;	/* Negative flag */

	uint32_t ptr[4];
} cpu_t;

/* cpu_new() -- create a cpu and give it a memory */
cpu_t *cpu_new(memory_t *mem);

/* cpu_destroy() -- destroy a cpu_t object (not its memory) */
void cpu_destroy(cpu_t *cpu);

/*** TODO
Understand the exact circumstances under which the carry flag is set
***/

#endif	/* CPU_H */
