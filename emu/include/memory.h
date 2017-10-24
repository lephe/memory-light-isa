#ifndef	MEMORY_H
#define	MEMORY_H

#include <stddef.h>
#include <stdint.h>

/* Default memory geometry, four segments of 1M each */
#define	MEMORY_DEFAULT_MEMSIZE	(4 << 20)
#define	MEMORY_DEFAULT_STACK	(2 << 20)
#define	MEMORY_DEFAULT_VRAMSIZE	(1 << 20)

typedef struct
{
	/* Memory geometry */
	uint32_t memsize;	/* Total memory size */
	uint32_t stack;		/* Bottom stack address */
	uint32_t vramsize;	/* Video RAM size */
	uint32_t data;		/* Data segment top */

	uint64_t *mem;

} memory_t;

/* memory_new() -- allocate a virtual memory */
memory_t *memory_new(uint32_t memsize, uint32_t stack, uint32_t vramsize);

/* memory_load() -- load a program into memory */
void memory_load(memory_t *mem, const char *filename);

/* memory_destroy() -- free a memory_t object allocated by memory_new() */
void memory_destroy(memory_t *mem);

//---
//	Data access
//---

/* memory_read() -- read n bits from an address (up to 64) */
uint64_t memory_read(memory_t *mem, uint32_t address, size_t n);

/*** TODO
int  Memory::read_bit(int ctr);
void Memory::write_bit(int ctr, int bit);
void Memory::set_counter(int ctr, uword val);
****/

#endif	/* MEMORY_H */
