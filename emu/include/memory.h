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
#define	MEMORY_DEFAULT_TEXT	(32 << 10)
#define	MEMORY_DEFAULT_STACK	(16 << 10)
#define	MEMORY_DEFAULT_DATA	(16 << 10)
#define	MEMORY_DEFAULT_VRAM	(327680)

/*
	memory_t structure
	This structure describes the geometry of the memory, which can be
	dynamically changed by the user to make sure the program fits in the
	provided address space.

	The emulated looks like this :
	     0000: Program          ^
	     text: Stack            |  memsize
	    stack: Data segment     |
	     vram: Video memory     v

	The stack grows down in the address space, so the 'stack' attribute
	represents the beginning address of the data segment.
*/
typedef struct
{
	uint64_t memsize;	/* Total memory size */
	uint64_t text;		/* Size of text segment */
	uint64_t stack;		/* Bottom stack address */
	uint64_t data;		/* Address of the data segment */
	uint64_t vram;		/* Address of the vram segment */

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

	@arg	text	Requested text segment size
	@arg	stack	Requested stack segment size
	@arg	data	Requested data segment size
	@arg	vram	Requested video memory size
	@returns	A new memory object. Program terminates on error.
*/
memory_t *memory_new(uint64_t text, uint64_t stack, uint64_t data,
	uint64_t vram);

/*
	memory_load_program() -- load a program into memory
	Loads the file named 'filename' into the given memory. The file may not
	fit into the code/stack segment, in which case this function fails and
	calls exit(1). This function may also fail is an error is encountered
	while trying to load the file. If everything succeeds, it sets the
	'text' attribute of the mem object to the size of the file (in bits).

	@arg	mem		Memory to load the file into
	@arg	filename	File to load
*/
void memory_load_program(memory_t *mem, const char *filename);

/*
	memory_load_text() -- load a text program into memory
	Same as memory_load_program(), but handles textual (ASCII) programs.

	@arg	mem		Memory to load the file into
	@arg	filename	Text program to load
*/
void memory_load_text(memory_t *mem, const char *filename);

/*
	memory_load_file() -- load an additional file into memory
	Loads the provided file at the given address in memory. The file must
	fit in memory. Returns non-zero on error.

	@arg	mem		Memory to load the file into
	@arg	address		Where to load the file into memory
	@arg	filename	File to load
*/
int memory_load_file(memory_t *mem, uint64_t address, const char *filename);

/*
	memory_destroy() -- free a memory_t object allocated by memory_new()

	@arg	mem	Memory object to destroy
*/
void memory_destroy(memory_t *mem);



//---
//	Data access
//---

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
