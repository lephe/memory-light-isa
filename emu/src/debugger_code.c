//---
//	emu:debugger:code - display disassembled code
//
//	This module is an interface-oriented component of the debugger. It
//	manages the disassembler windows, choosing what should be displayed and
//	where. It does not actually performs the disassembling; see emu:disasm
//	for this part.
//---

#define	_DEBUGGER_SOURCE
#include <string.h>
#include <ncurses.h>
#include <disasm.h>
#include <debugger.h>
#include <breaks.h>

/* Some hints about the geometry of the disassembled output. A disassembled
   line looks like this:

     0000ab:       jumpif     nz 0x0f     ; 0000ba                  PC
     ^^^^^^^       ^^^^^^     ^^^^^^^     ^^^^^^^^                  ^^
     Information   Mnemonic   Arguments   Complementary info        PC Counter

   If the disassembler window is too small, the complementary info isn't shown.
   Sometimes it's shown only if arguments do not contain 64-bit values, because
   these are rare. If the disassembler window is *really* small, the address
   information at the beginning is also left out. Finally, if the instruction
   is currently being pointed at by PC, "PC" appears at the far right. */

static int geom_addr = -1;	/* Length of memory addresses */
static int geom_info = -1;	/* Position of left information */
static int geom_base = -1;	/* Position of mnemonic and arguments */
static int geom_comp = -1;	/* Position of complementary information */
static int geom_cmpl = -1;	/* Do we have enough space for complementary
				   information after a literal 64-bit value? */

/* Disassembling mode:
   - If mode_pc = 1, the disassembler follows pc
   - Otherwise, the disassembler shows fixed address mode_addr */
static int mode_pc = 1;
static uint64_t mode_addr = 0;

/*
	debugger_arg()
	Retrieve and print an argument on the current row. Displays
	complementary information if this is appropriate. Also updates the
	pointer to the emulated memory, which was pointing to the argument.

	@arg	ptr	Pointer to current location counter (ie. PC), updated
	@arg	type	Argument type
*/
static void debugger_arg(uint64_t *ptr, arg_t type)
{
	/* TODO - Bad design here, it is not this function's responsibility to
	   print the complementary information.
	   Problem is, the caller has almost zero knowledge of the printed
	   instruction's nature */

	/* Names for pointer and condition arguments */
	const char *ptr_names[4] = { "pc", "sp", "a0", "a1" };
	const char *cnd_names[8] = {
		"z", "nz", "sgt", "slt", "gt", "nc", "c", "v"
	};

	/* Arguments retrieved from the disasm module */
	uint gen;
	int64_t i64;
	uint64_t u64;

	/* Calculation-related variables */
	const char *sign;	/* Pointer to "-" or "" for signed hexa */
	uint size;		/* Size of argument constants */

	/* Position of the cursor */
	size_t y, __attribute__((unused)) x;
	/* A naming shorthand */
	memory_t *mem = debugger_mem;

	switch(type)
	{
	case arg_none:
		return;

	case arg_reg:
		wprintw(wcode, " r%d", disasm_reg(mem, ptr));
		return;

	case arg_dir:
		gen = disasm_dir(mem, ptr);
		wprintw(wcode, " %s", gen ? "right" : "left");
		return;

	case arg_cond:
		wprintw(wcode, " %s", cnd_names[disasm_cond(mem, ptr)]);
		return;

	case arg_addr:
		/* Get the address, display it with the proper sign */
		i64 = disasm_addr(mem, ptr, &size);
		if(i64 < 0) sign = "-", u64 = -i64;
		else sign = "", u64 = i64;
		wprintw(wcode, " %s0x%0*lx", sign, size >> 2, u64);

		/* Compute the target address for jumps */
		i64 += *ptr;

		/* Determine whether we display it or not (geometry) */
		if(geom_comp == -1) return;
		/* Avoid 64-bit addresses on small screens */
		if((i64 > 0xffffffffl || i64 < 0) && geom_cmpl == 0) return;

		/* Display the target as complementary info if possible */
		wattron(wcode, A_DIM);
		getyx(wcode, y, x);
		mvwprintw(wcode, y, geom_comp, " ; %0*lx", geom_addr, i64);
		wattroff(wcode, A_DIM);
		return;

	case arg_lconst:
		u64 = disasm_lconst(mem, ptr, &size);
		wprintw(wcode, " 0x%0*lx", size >> 2, u64);
		return;

	case arg_aconst:
		i64 = disasm_aconst(mem, ptr, &size);
		/* Display "small" constants as decimal */
		if(size <= 32) wprintw(wcode, " %ld", i64);
		else
		{
			if(i64 < 0) sign = "-", i64 = -i64;
			else sign = "";
			wprintw(wcode, " %s0x%0*lx", sign, size >> 2, i64);
		}
		return;

	case arg_shift:
		wprintw(wcode, " %d", disasm_shift(mem, ptr));
		return;

	case arg_size:
		wprintw(wcode, " %d", disasm_size(mem, ptr));
		return;

	case arg_pointer:
		gen = disasm_pointer(mem, ptr);
		wprintw(wcode, " %s", ptr_names[gen]);
		return;
	}
}

/*
	debugger_disasm()
	Disassemble code starting from a given memory location and show it in
	the code panel.
*/
static void debugger_disasm(uint64_t ptr)
{
	const char *format;	/* Instruction format, see disasm.c */
	uint64_t copy;		/* Copy of the location counter */
	size_t row;		/* Row allocated to the current instruction */

	/* Dimensions of the code window */
	size_t h, __attribute__((unused)) w;
	getmaxyx(wcode, h, w);

	for(size_t i = 0; i < h - 2; i++)
	{
		row = i + 1;
		copy = ptr;

		/* Leave if we've reached the end of the text segment */
		if(ptr >= debugger_mem->text) break;

		/* Get an opcode from the current program counter */
		disasm_opcode(debugger_mem, &ptr, &format);

		/* Display instruction addresses if there's enough space */
		if(geom_info >= 0)
		{
			int style = break_has(copy)
				? COLOR_PAIR(color_red)
				: A_DIM | COLOR_PAIR(color_white);

			wmove(wcode, row, geom_info);
			wattron(wcode, style);
			wprintw(wcode, "%0*x:  ", geom_addr, copy);
			wattroff(wcode, style);
		}

		/* Show the mnemonic with proper syntax highlighting */
		int color = color_white;
		if(format[4] == 'A') color = color_arithm;
		if(format[4] == 'T') color = color_test;
		if(format[4] == 'L') color = color_let;
		if(format[4] == 'J') color = color_jump;
		if(format[4] == 'M') color = color_memory;
		if(format[4] == 'C') color = color_control;

		wattron(wcode, COLOR_PAIR(color));
		mvwprintw(wcode, row, geom_base, "%-8s", format + 6);
		wattroff(wcode, COLOR_PAIR(color));

		/* Display arguments, if any */
		for(size_t j = 0; j < 3; j++)
			debugger_arg(&ptr, format[j]);

		/* If this instruction is currently being pointed at by the
		   program counter, highlight it */
		if(copy == debugger_cpu->ptr[PC])
		{
			mvwaddstr(wcode, row, w - 4, "PC");
			mvwchgat(wcode, row, 1, w - 1, A_REVERSE, color_white,
				NULL);
		}
	}
}

/* debugger_code_mode() -- set the disassembler mode */
void debugger_code_mode(int follow_pc, uint64_t address)
{
	mode_pc = follow_pc;
	mode_addr = address;
}

/* debugger_code() -- refresh the code panel, showing disassembled code */
void debugger_code(void)
{
	/* Border, title and stuff */

	wclear(wcode);
	wborder(wcode, 0, ' ', 0, ' ', 0, ACS_HLINE, ACS_VLINE, ' ');

	wattron(wcode, A_BOLD);
	mvwaddstr(wcode, 0, 1, " Disassembled code ");
	wattroff(wcode, A_BOLD);

	/* More importantly, the disassembled code */
	debugger_disasm(mode_pc ? debugger_cpu->ptr[PC] : mode_addr);
	wrefresh(wcode);
}

/* debugger_code_init() -- compute the geometry of the disassembled output */
void debugger_code_init(void)
{
	int __attribute__((unused)) h, w;
	getmaxyx(wcode, h, w);

	/* Leave some margins for the window borders */
	w -= 5;
	/* Also leave a few characters to write "PC" */
	w -= 3;

	/* Size of text section hexadecimal addresses */
	geom_addr = 6;
	if(debugger_mem->text > 0xffffffff)	geom_addr = 16;
	if(debugger_mem->text > 0xffffff)	geom_addr = 8;
	if(debugger_mem->text <= 0xffff)	geom_addr = 4;

	/* Column where mnemonics start */
	if(w < geom_addr + 25) geom_info = -1, geom_base = 3;
	else geom_info = 3, geom_base = 3 + geom_addr + 3;

	/* Column where complementary information starts */

	/* Boundary 1 is the width required to display a complementary address
	   given that all immediate values are no more than 32 bits */
	int boundary1 = geom_addr + 29 + 8;
	/* Boundary 2 is the same when there are 64-bit immediate values */
	int boundary2 = geom_addr + 37 + 16;

	/* Set geom_comp and gomp_cmpl accordingly */
	if(w < boundary1) geom_comp = -1;
	else if(w < boundary2) geom_comp = 3 + geom_addr + 27, geom_cmpl = 0;
	else geom_comp = 3 + geom_addr + 35, geom_cmpl = 1;
}
