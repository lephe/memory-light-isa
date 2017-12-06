#include <stdlib.h>
#include <stdio.h>
#include <endian.h>

#include <memory.h>
#include <errors.h>
#include <graphical.h>

/* memory_new() -- allocate a virtual memory() */
memory_t *memory_new(uint64_t text, uint64_t stack, uint64_t data,
	uint64_t vram)
{
	/* Using default parameters if some are not provided */
	if(!text)	text	= MEMORY_DEFAULT_TEXT;
	if(!stack)	stack	= MEMORY_DEFAULT_STACK;
	if(!data)	data	= MEMORY_DEFAULT_DATA;
	if(!vram)	vram	= MEMORY_DEFAULT_VRAM;

	/* Checking the consistency of all values */
	if((text & 63) || (stack & 63) || (data & 63) || (vram & 63))
		fatal("all segment sizes ought to be multiples of 64");
	if(text + stack + data != 0x10000)
		warn("chosen vram location is incompatible with the standard");
	if(vram < 0x50000)
		warn("vram is too small for the standard");

	uint64_t memsize = text + stack + data + vram;

	/* Allocating the structure and the data array in one go. This requires
	   an alignment trick because we'd also like the memory pointer to be
	   8-aligned. We thus make sure the size of memory_t is 8-padded */
	size_t base = (sizeof(memory_t) + 7) & ~7;
	size_t size = base + ((memsize + 7) >> 3);

	void *memdata = calloc(size, 1);
	if(!memdata) ifatal("# memory_new(): cannot allocate memory");

	memory_t *mem = memdata;

	/* Filling in the structure fields */
	mem->memsize	= memsize;
	mem->text	= text;
	mem->stack	= text + stack;
	mem->data	= text + stack;
	mem->vram	= text + stack + data;
	mem->mem	= memdata + base;

	return mem;
}

/* memory_load_program() -- load a program into memory */
void memory_load_program(memory_t *mem, const char *filename)
{
	/* Sanity checks */
	if(!mem || !filename)
		ifatal("memory_load_program(): NULL memory or filename");

	/* Open the file for reading */
	error_clear();
	FILE *fp = fopen(filename, "r");
	if(!fp) fatal("# cannot open '%s'", filename);

	/* Retrieve the file size by seeking */
	fseek(fp, 0, SEEK_END);
	unsigned long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	/* Remember that there is an 8-byte header at the beginning */
	if(size <= 8)
	{
		error("binary file is either empty or corrupted");
	}
	uint64_t header_text;
	size -= 8;

	/* Check that it fits into memory */
	if(8 * size > mem->text)
	{
		error("program is too large to fit in the code/stack segment "
			"(%d > %d)", 8 * size, mem->text);
		note("you may want to change the memory geometry using "
			"--geometry");
	}

	/* Read the data and close the file */
	else
	{
		size_t b1 = fread(&header_text, 8, 1, fp);
		if(b1 < 1) error("# cannot read from '%s'",filename);

		/* Does the file announces more data that what it actually
		   contains? */
		if(8 * size < be64toh(header_text))
		{
			error("binary file is corrupted or data is missing");
		}
		else
		{
			size_t b2 = fread(mem->mem, size, 1, fp);
			if(b2 < 1) error("# cannot read from '%s'",filename);
		}
	}
	fclose(fp);
	error_check();

	/* Set the length of the text section in the memory object */
	mem->text = be64toh(header_text);
}

/* memory_load_file() -- load an additional file into memory */
int memory_load_file(memory_t *mem, uint64_t address, const char *filename)
{
	if(!mem || !filename)
		ifatal("memory_load_file(): NULL memory or filename");

	/* Check that we're not *already* out of bounds */
	if(address >= mem->memsize)
	{
		error("load address of '%s' is out of memory (%#x >= %#x)",
			filename, address, mem->memsize);
		return 1;
	}

	/* Check that we're loading on a byte boundary */
	if(address & 7)
	{
		error("can only load files on byte boundaries");
		return 1;
	}

	/* Open and read the file */

	FILE *fp = fopen(filename, "r");
	if(!fp)
	{
		error("# cannot open '%s'", filename);
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	unsigned long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	/* Check some more bounds */
	if(address + size > mem->memsize)
	{
		error("load area of '%s' overflows from memory (%#x + %d > "
			"%#x)", filename, address, size, mem->memsize);
		fclose(fp);
		return 1;
	}

	int x = fread((void *)mem->mem + (address >> 3), size, 1, fp);
	fclose(fp);

	if(!x) error("# cannot read from '%s'", filename);
	return !x;
}

/* memory_destroy() -- free a memory_t object allocated by memory_new() */
void memory_destroy(memory_t *mem)
{
	if(!mem) return;

	/* We don't need to free memory->data because the structure and the
	   data array were allocated in a single chunk */
	free(mem);
}



//---
//	Data access
//---

/* memory_read() -- read n bits from an address (up to 64) */
uint64_t memory_read(memory_t *mem, uint64_t address, size_t n)
{
	/* Basic sanity checks */
	if(!mem) ifatal("memory_read(): NULL memory");
	if(n > 64) fatal("memory_read(): Too much data requested (%u > 64)",n);

	/* TODO: memory_read() - use an exception when out of bounds */
	if(address + n > mem->memsize) fatal("memory_read(): Out of bounds "
		"(%x:%u > %x)", address, n, mem->memsize);

	if(!n) return 0;

	uint64_t base = address >> 6;
	int right = (address & 63) + n;

	/* If requested data does not overlap two 64-bit words, get it in a
	   single array access */
	if(right <= 64)
	{
		uint64_t word = htobe64(mem->mem[base]);

		/* In some cases the shift below U.B., so we check */
		if(n == 64) return word;
		return (word >> (64 - right)) & ((1ul << n) - 1);
	}

	/* Otherwise, proceed in two steps and join the results */
	else
	{
		right -= 64;	/* Number of bits from w2 */

		uint64_t w1 = htobe64(mem->mem[base]);
		uint64_t w2 = htobe64(mem->mem[base + 1]);

		w1 = w1 & ((1ul << (n - right)) - 1);
		w2 = w2 >> (64 - right);

		return (w1 << right) | w2;
	}
}

/* memory_write() -- write n bits to an address (up to 64) */
/* TODO: Check that this function actually works */
void memory_write(memory_t *mem, uint64_t address, uint64_t x, size_t n)
{
	/* Sanity checks */
	if(!mem) ifatal("memory_write(): NULL memory");
	if(n > 64)fatal("memory_write(): Too much data requested (%u > 64)",n);

	/* TODO: memory_write() - use an exception when out of bounds */
	if(address + n > mem->memsize) fatal("memory_write: Out of bounds "
		"(%x:%u > %x)", address, n, mem->memsize);

	if(!n) return;

	/* Remove additional unwanted bits and be careful about U.B. */
	if(n < 64) x &= (1ul << n) - 1;

	uint64_t base = address >> 6;
	int right = (address & 63) + n;

	/* If the target area is on a single 64-bit word, write it in one go */
	if(right <= 64)
	{
		right = 64 - right;	/* Number of free bits at the right */

		uint64_t word = htobe64(mem->mem[base]);
		uint64_t mask = (n < 64) ? ((1ul << n) - 1) : (uint64_t)(-1);
		mask <<= right;

		word = (word & ~mask) | (x << right);
		mem->mem[base] = be64toh(word);
	}

	/* Otherwise, proceed in two steps */
	else
	{
		uint64_t w1 = htobe64(mem->mem[base]);
		uint64_t w2 = htobe64(mem->mem[base + 1]);
		right -= 64;

		w1 = (w1 & ~((1ul << (n - right)) - 1)) | (x >> right);
		w2 = ((w2 << right) >> right) | (x << (64 - right));

		mem->mem[base]     = be64toh(w1);
		mem->mem[base + 1] = be64toh(w2);
	}

#ifdef GRAPHICAL_ASYNC
	/* Check whether the video memory was affected, sending a graphical
	   refresh when required */
	if(address + n > mem->vram) graphical_refresh();
#endif
}
