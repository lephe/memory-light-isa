#include <stdlib.h>
#include <stdio.h>
#include <endian.h>

#include <memory.h>
#include <errors.h>

/* memory_new() -- allocate a virtual memory() */
memory_t *memory_new(uint32_t memsize, uint32_t stack, uint32_t vramsize)
{
	/* Using the default parameters */
	if(!memsize)	memsize  = MEMORY_DEFAULT_MEMSIZE;
	if(!stack)	stack    = MEMORY_DEFAULT_STACK;
	if(!vramsize)	vramsize = MEMORY_DEFAULT_VRAMSIZE;

	/* Checking the consistency of the provided geometry */
	error_clear();

	if(stack > memsize)
		fatal("inconsistent memory layout, no space left for stack");
	if(stack + vramsize > memsize)
		fatal("inconsistent memory layout, no space left for video "
		"memory");
	if(stack + vramsize == memsize)
		warn("no space left for the data segment");

	error_check();

	/* Allocating the structure and the data array in one go. This requires
	   an alignment trick because we'd also like the memory pointer to be
	   8-aligned. We thus make sure the size of memory_t is 8-padded */
	size_t base = (sizeof(memory_t) + 7) & ~7;
	size_t size = base + ((memsize + 7) >> 3);
	void *data = malloc(size);

	if(!data) ifatal("# memory_new(): cannot allocate memory");
	memory_t *mem = data;

	/* Filling in the structure fields */
	mem->memsize	= memsize;
	mem->text	= 0x000000;
	mem->stack	= stack;
	mem->vramsize	= vramsize;
	mem->data	= stack + vramsize;
	mem->mem	= data + base;

	return mem;
}

/* memory_load() -- load a program into memory */
void memory_load(memory_t *mem, const char *filename)
{
	if(!mem || !filename) ifatal("memory_load(): NULL memory or filename");

	error_clear();
	FILE *fp = fopen(filename, "r");
	if(!fp) fatal("# cannot open '%s'", filename);

	fseek(fp, 0, SEEK_END);
	unsigned long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if(size > mem->stack)
	{
		error("program is too large to fit in the code/stack segment "
			"(%d > %d)", size, mem->stack),
		fclose(fp);
	}

	size_t blocks = fread(mem->mem, size, 1, fp);
	fclose(fp);

	if(blocks < 1) error("# cannot read from '%s'", filename);
	error_check();

	mem->text = 8 * size;
}

/* memory_destroy() -- free a memory_t object allocated by memory_new() */
void memory_destroy(memory_t *mem)
{
	if(!mem) return;

	/* We don't need to free memory->data because the structure and the
	   data array are allocated in a single chunk */
	free(mem);
}

/* memory_read() -- read n bits from an address (up to 64) */
uint64_t memory_read(memory_t *mem, uint32_t address, size_t n)
{
	if(!mem) ifatal("memory_read(): NULL memory");

	/* TODO - Use an event queue (or sth like that) to set up a properly-
	   managed exception system for the debugger */
	if(address + n > mem->memsize) fatal("memory_read(): Out of bounds "
		"(%x/%d > %x)", address, n, mem->memsize);

	uint32_t base = address >> 6;
	int right = (address & 63) + n;

	/* If requested data does not overlap two 64-bit words, get it */
	if(right <= 64)
	{
		uint64_t word = htobe64(mem->mem[base]);
		return (word >> (64 - right)) & ((1 << n) - 1);
	}

	/* Otherwise, proceed in two steps */
	else
	{
		right -= 64;	/* Number of bits from w2 */

		uint64_t w1 = htobe64(mem->mem[base]);
		uint64_t w2 = htobe64(mem->mem[base + 1]);

		w1 = w1 & ((1 << (n - right)) - 1);
		w2 = w2 >> (64 - right);

		return (w1 << right) | w2;
	}
}
