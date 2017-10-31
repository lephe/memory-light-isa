#include <disasm.h>
#include <stdint.h>

#define	r4(n)	n, n, n, n
#define	r8(n)	r4(n), r4(n)

static const uint8_t ids[128] = {
	r8(0), r8(1), r8(2),  r8(3),  r8(4),  r8(5),  r8(6),  r8(7),
	r8(8), r4(9), r4(10), r8(11), r8(12),
	13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20,
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
};
static const uint8_t length[37] = {
	r8(4), 4, 5, 5, 4, 4, r8(6), r8(7), r8(7)
};

static const char instructions[37][16] = {
	"rr- A add2",	"rl- A add2i",	"rr- A sub2",	"rl- A sub2i",
	"rr- T cmp",	"rc- T cmpi",	"rr- L let",	"rc- L leti",
	"drh A shift",	"psr M readze",	"psr M readse",	"a-- J jump",
	"oa- J jumpif",	"rr- A or2",	"rl- A or2i",	"rr- A and2",
	"rl- A and2i",	"psr M write",	"a-- J call",	"pr- C setctr",
	"pr- C getctr",	"sr- M push",	"--- J return",	"rrr A add3",
	"rrl A add3i",	"rrr A sub3",	"rrl A sub3i",	"rrr A and3",
	"rrl A and3i",	"rrr A or3",	"rrl A or3i",	"rrr A xor3",
	"rrl A xor3i",	"rrh A asr3",	"--- C (res)",	"--- C (res)",
	"--- C (res)",
};

static inline int64_t sign_extend(uint64_t x, uint size)
{
	uint64_t m = 1u << (size - 1);
	return (x ^ m) - m;
}

uint disasm_opcode(memory_t *mem, uint32_t *ptr, const char **format)
{
	uint32_t opcode = memory_read(mem, *ptr, 7) & 0x7f;
	uint id = ids[opcode & 0x7f];

	*ptr += length[id];
	if(format) *format = instructions[id];

	return id;
}

uint disasm_reg(memory_t *mem, uint32_t *ptr)
{
	*ptr += 3;
	return memory_read(mem, *ptr - 3, 3);
}

uint disasm_dir(memory_t *mem, uint32_t *ptr)
{
	*ptr += 1;
	return memory_read(mem, *ptr - 1, 1);
}

uint disasm_cond(memory_t *mem, uint32_t *ptr)
{
	*ptr += 3;
	return memory_read(mem, *ptr - 3, 3);
}

int64_t disasm_addr(memory_t *mem, uint32_t *ptr, uint *size_arg)
{
	/* Length of header, size of address, 3 header bits of address */
	uint offset = 1, size = 8;
	uint head = memory_read(mem, *ptr, 3);

	if(head == 4 || head == 5) offset = 2, size = 16;
	else if(head >= 6) offset = 3, size = 1 << (head - 1);

	*ptr += offset + size;
	if(size_arg) *size_arg = size;
	uint64_t addr = memory_read(mem, *ptr - size, size);

	return sign_extend(addr, size);
}

#include <stdio.h>
uint64_t disasm_lconst(memory_t *mem, uint32_t *ptr, uint *size_arg)
{
	/* Length of header, size of constant, 3 header bits of constant */
	uint offset = 1, size = 1;
	uint head = memory_read(mem, *ptr, 3);

	if(head == 4 || head == 5) offset = 2, size = 8;
	else if(head >= 6) offset = 3, size = 1 << (head - 1);

	*ptr += offset + size;
	if(size_arg) *size_arg = size;

	uint64_t t = memory_read(mem, *ptr - size, size);
	return t;
}

int64_t disasm_aconst(memory_t *mem, uint32_t *ptr, uint *size_arg)
{
	uint size;
	uint64_t cst = disasm_lconst(mem, ptr, &size);
	if(size_arg) *size_arg = size;
	return sign_extend(cst, size);
}

uint disasm_shift(memory_t *mem, uint32_t *ptr)
{
	uint shift = memory_read(mem, *ptr, 7);
	if(shift & 0x40) shift = 1, (*ptr)++;
	else *ptr += 7;
	return shift;
}

uint disasm_size(memory_t *mem, uint32_t *ptr)
{
	uint size = memory_read(mem, *ptr, 3);
	*ptr += 2 + (size >= 4);

	if(size <= 3) return 1 + 3 * (size >> 1);
	return 1 << (size - 1);
}

uint disasm_pointer(memory_t *mem, uint32_t *ptr)
{
	*ptr += 2;
	return memory_read(mem, *ptr - 2, 2);
}
