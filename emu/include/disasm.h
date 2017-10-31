//---
//	emu:disasm - disassemble instructions from the memory
//
//	This module provides random-access disassembling and easy parsing of
//	instructions from the memory.
//---

#ifndef	DISASM_H
#define	DISASM_H

#include <stdint.h>
#include <stddef.h>

#include <defs.h>
#include <memory.h>

/*
	arg_t enumeration
	Kinds of arguments that may be passed to instructions. Each instruction
	has at most 3 arguments. These characters are used in the format string
	returned by disasm_opcode().
*/
typedef enum
{
	arg_none	= '-',	/* No argument */
	arg_reg		= 'r',	/* Register: r0..r7 on 3 bits */
	arg_dir		= 'd',	/* Direction: left/right on 1 bit */
	arg_cond	= 'o',	/* Condition: various on 3 bits */
	arg_addr	= 'a',	/* Address: on 9, 18, 35 or 67 bits */
	arg_lconst	= 'l',	/* Constants: on 2, 18, 35 or 67 bits */
	arg_aconst	= 'c',	/* Arithmetic, ie. signed, constants */
	arg_shift	= 'h',	/* Shifts: 1b on 1 bit, or 7 bits */
	arg_size	= 's',	/* Size: 2 or 3 bits */
	arg_pointer	= 'p',	/* Pointer: PC, SP, A0 or A1 on 2 bits */
} arg_t;

/*
	ctgy_t enumeration
	Instructions categories. This classification is only used by the
	debugger for highlighting purposes.
*/
typedef enum
{
	ctgy_arithm	= 'A',
	ctgy_test	= 'T',
	ctgy_let	= 'L',
	ctgy_jump	= 'J',
	ctgy_memory	= 'M',
	ctgy_control	= 'C',
} ctgy_t;



//---
//	Disassembler functions
//
//	The following functions all read data from the memory and return a
//	relevant value. They are provided with two base arguments, the memory
//	to read, and a pointer to a memory location where data will be read.
//	The pointer is updated after reading:
//
//	@arg	mem	Memory to inspect
//	@arg	ptr	Pointer to code-pointer, update after reading
//----

/*
	disasm_opcode() -- read an instruction code
	Reads an opcode from the memory and advances the given pointer. Returns
	the opcode id and sets the instruction format if non-NULL. The
	instruction format is a string on the form
		"<arg1><arg2><arg3> <category> <mnemonic>"
	where arg1, arg2 and arg3 are characters from the arg_t enumeration,
	and category is from the ctgy_t enumeration.

	@arg	format	Pointer to format string, set if non-NULL
	@returns	The identifier of the first opcode available at *ptr
*/
uint disasm_opcode(memory_t *mem, uint64_t *ptr, const char **format);

/*
	disasm_reg() -- read a register number
	@returns	Register number on 3 bits
*/
uint disasm_reg(memory_t *mem, uint64_t *ptr);

/*
	disasm_dir() -- read a shift direction bit
	@returns	Direction bit: 0 for left, 1 for right
*/
uint disasm_dir(memory_t *mem, uint64_t *ptr);

/*
	disasm_cond() -- read a jump condition type
	@returns	Condition kind on 3 bits (eq neq sgt slt gt ge lt le)
*/
uint disasm_cond(memory_t *mem, uint64_t *ptr);

/*
	disasm_addr() -- read a relative address
	An optional pointer parameter may be passed. If non-NULL, it is set to
	the number of bits in the processor representation of the address.

	@arg	size	Optional pointer to address size, set if non-NULL
	@returns	Relative address
*/
int64_t disasm_addr(memory_t *mem, uint64_t *ptr, uint *size);

/*
	disasm_lconst() -- read a zero-extended constant
	disasm_aconst() -- read a sign-extended constant

	An optional pointer may be passed to retrieve the data size. The
	constant is extended if required.

	@arg	size	Optional pointer to constant size, set if non-NULL
	@returns	Constant read and extended
*/
uint64_t disasm_lconst(memory_t *mem, uint64_t *ptr, uint *size);
 int64_t disasm_aconst(memory_t *mem, uint64_t *ptr, uint *size);

/*
	disasm_shift() -- read a shift constant
	@returns	Unsigned shift constant on 6 bits
 */
uint disasm_shift(memory_t *mem, uint64_t *ptr);

/*
	disasm_size() -- read a memory operation size
	@returns	Operation size: one of 1, 4, 8, 16, 32 and 64
*/
uint disasm_size(memory_t *mem, uint64_t *ptr);

/*
	disasm_pointer() -- read a pointer id
	@returns	Pointer id on two bits: in order, PC, SP, A0, A1
*/
uint disasm_pointer(memory_t *mem, uint64_t *ptr);

#endif	/* DISASM_H */
