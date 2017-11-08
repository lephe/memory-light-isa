//---
//	emu:memory - emulate a random-access bit-addressable memory
//
//	This module provides routines for manipulating the bit-addressable
//	memory used by the fictional CPU. Instead of emulating the four memory
//	counters (or pointers, as I'd prefer to name them) and the serial
//	line between CPU and memory, I opted for a simple design with random
//	access of at most 64 bits at a time.
//
//	There are several reasons for this choice, the main ones being
//	flexibility and performance. The debugger needs to disassemble code
//	independently of the emulated processor's PC, and keeping and updating
//	copies of the counter in the memory object would only make this a lot
//	more difficult. Reading only a single bit at a time is also a clear
//	performance bottleneck (not the only one, though).
//---

#ifndef	MEMORY_H
#define	MEMORY_H

#include <stddef.h>
#include <stdint.h>

/* Default memory geometry, four segments of 1M each */
#define	MEMORY_DEFAULT_MEMSIZE	(4 << 20)
#define	MEMORY_DEFAULT_STACK	(2 << 20)
#define	MEMORY_DEFAULT_VRAMSIZE	(1 << 20)

/*
	memory_t structure
	This structure describes the geometry of the memory, which can be
	dynamically changed by the user to make sure the program fits in the
	provided address space.

	The emulated looks like this :
	     0000: Program          ^
	           Stack            |  memsize
	    stack: Video memory     |
	     data: Data segment     v

	The stack grows down in the address space, which is why the stack
	attribute actually represents the beginning address of the video
	memory.
*/
typedef struct
{
	uint64_t memsize;	/* Total memory size */
	uint64_t text;		/* Text segment size */
	uint64_t stack;		/* Bottom stack address */
	uint64_t vramsize;	/* Size of the video memory segment */
	uint64_t data;		/* Top address of the data segment */

	/* Pointer to actual chunk of data, of size memsize */
	uint64_t *mem;

} memory_t;

//---
//	Memory object management
//---

/*
	memory_new() -- allocate a virtual memory
	This function checks that the memory layout provided through the
	parameters is consistent with the memory model, and creates a new
	memory object with freshly allocated data.

	Some parameters may not be provided (when 0 is passed), in this case
	the default values are used (see the macros above).

	@arg	memsize		Requested total memory size
	@arg	stack		Request bottom stack address
	@arg	vramsize	Request video memory size
	@returns		A new memory object. Calls exit(1) on error.
*/
memory_t *memory_new(uint64_t memsize, uint64_t stack, uint64_t vramsize);

/*
	memory_load() -- load a program into memory
	Loads the file named 'filename' into the given memory. The file may not
	fit into the code/stack segment, in which case this function fails and
	calls exit(1). This function may also fail is an error is encountered
	while trying to load the file. If everything succeeds, it sets the
	'text' attribute of the mem object to the size of the file (in bits).

	@arg	mem		Memory to load the file into
	@arg	filename	File to load
*/
void memory_load(memory_t *mem, const char *filename);

/*
	memory_destroy() -- free a memory_t object allocated by memory_new()

	@arg	mem	Memory object to destroy
*/
void memory_destroy(memory_t *mem);



//---
//	Data access
//---

/*
	mem_at_end() -- check whether the end of the text segment is reached
	Returns non-zero if no more instruction can be read from the given
	program counter without overrunning the text segment.
	This function exists because of the necessity to properly handle
	byte-padding at the end of the binary file, and the influence of the
	dynamic Huffman encoding on this detection.
*/
int mem_at_end(memory_t *mem, uint64_t pc);

/*
	memory_read() -- read n bits from an address (up to 64)
	Reads up to 64 bits from the given address. Data is returned in
	"big-endian order".

	@arg	mem	Memory to read from
	@arg	address	Address of the first bit to read
	@arg	n	Number of bits to read (at most 64)
*/
uint64_t memory_read(memory_t *mem, uint64_t address, size_t n);

/*
	memory_write() -- write n bits to an address (up to 64)
	Writes up to 64 bits to the given address. The lower bits of the given
	value are used. They are supposed to be in "big-endian order".

	@arg	mem	Memory to write to
	@arg	address	Address of the first bit to overwrite
	@arg	x	Variable containing raw data
	@arg	n	Number of significant bits in x (at most 64)
*/
void memory_write(memory_t *mem, uint64_t address, uint64_t x, size_t n);

#endif	/* MEMORY_H */
