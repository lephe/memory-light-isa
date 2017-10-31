#define	_DEBUGGER_SOURCE
#include <string.h>
#include <ncurses.h>
#include <disasm.h>
#include <debugger.h>

/* Some hints about the geometry of the disassembled output */
static int geom_addr = -1;	/* Size of text section addresses */
static int geom_info = -1;	/* Position of left information (addresses) */
static int geom_base = -1;	/* Position of mnemonics */
static int geom_comp = -1;	/* Position of complementary information */
static int geom_cmpl = -1;	/* Do we have enough space for complementary
				   information after a literal 64-bit value? */

static void debugger_arg(uint32_t *ptr, int type, uint32_t row, uint32_t pc)
{
	const char *ptr_names[4] = { "pc", "sp", "a0", "a1" };
	const char *cnd_names[8] = {
		"z", "nz", "sgt", "slt", "gt", "nc", "c", "le"
	};
	const char *sign;
	uint gen, size;
	int64_t i64;
	uint64_t u64;
	size_t __attribute__((unused)) dummy, x, w;

	memory_t *mem = debugger_mem;	/* Naming shorthand */

	switch(type)
	{
	case arg_none: return;
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
		i64 = disasm_addr(mem, ptr, &size);
		if(i64 < 0) sign = "-", u64 = -i64;
		else sign = "", u64 = i64;
		wprintw(wcode, " %s0x%0*lx", sign, size >> 2, u64);
		/* Display the target address if there's enough space left */
		getyx(wcode, dummy, x);
		getmaxyx(wcode, dummy, w);
		if(geom_comp == -1) return;
		/* Avoid 64-bit addresses on small screens */
		i64 += (uint64_t)pc;
		if((i64 > 0xffffffffL || i64 < 0) && geom_cmpl == 0) return;
		wattron(wcode, A_DIM);
		mvwprintw(wcode, row, geom_comp, " ; %0*lx", geom_addr, i64);
		wattroff(wcode, A_DIM);
		return;
	case arg_lconst:
		u64 = disasm_lconst(mem, ptr, &size);
		wprintw(wcode, " 0x%0*lx", size >> 2, u64);
		return;
	case arg_aconst:
		i64 = disasm_aconst(mem, ptr, &size);
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

static void debugger_disasm(void)
{
	uint32_t ptr = debugger_cpu->ptr[PC], copy;
	const char *format;
	size_t row;

	size_t h, __attribute__((unused)) w;
	getmaxyx(wcode, h, w);

	for(size_t i = 0; i < h - 2; i++)
	{
		row = i + 1;
		copy = ptr;

		disasm_opcode(debugger_mem, &ptr, &format);

		/* We need to determine the location where the text segment
		   ends. Because of file byte-padding, up to 7 additional bits
		   may exist, and some instructions fit in 7 bits. */

		/* TODO FIXME - I don't like this
		   Leaving if the 7-bit instruction has any arguments */
		if(ptr + 7 >= debugger_mem->text && strncmp(format, "---", 3))
			break;

		if(geom_info >= 0)
		{
			wmove(wcode, row, geom_info);
			wattron(wcode, A_DIM | COLOR_PAIR(color_white));
			wprintw(wcode, "%0*x:  ", geom_addr, copy);
			wattroff(wcode, A_DIM | COLOR_PAIR(color_white));
		}

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

		for(size_t j = 0; j < 3; j++)
			debugger_arg(&ptr, format[j], row, copy);

		if(copy == debugger_cpu->ptr[PC])
		{
			mvwaddstr(wcode, row, w - 4, "PC");
			mvwchgat(wcode, row, 1, w - 1, A_REVERSE, color_white,
				NULL);
		}
	}
}

/* debugger_code() -- display disassembled code in the wcode panel */
void debugger_code(void)
{
	wclear(wcode);
	wborder(wcode, 0, ' ', 0, ' ', 0, ACS_HLINE, ACS_VLINE, ' ');

	wattron(wcode, A_BOLD);
	mvwaddstr(wcode, 0, 1, " Disassembled code ");
	wattroff(wcode, A_BOLD);

	debugger_disasm();
	wrefresh(wcode);
}

/* debugger_code_init() -- compute the geometry of the disassembled output */
void debugger_code_init(void)
{
	/* Leave some margins in the computation */
	int __attribute__((unused)) h, w;
	getmaxyx(wcode, h, w);
	w -= 5;

	/* Also leave a few characters to write "PC" */
	w -= 3;

	/* Size of text section hexadecimal addresses */
	geom_addr = 6;
	if(debugger_mem->text > 0xffffff) geom_addr = 8;
	if(debugger_mem->text <= 0xffff)  geom_addr = 4;

	/* Column where mnemonics start */
	if(w < geom_addr + 25) geom_info = -1, geom_base = 3;
	else geom_info = 3, geom_base = 3 + geom_addr + 3;

	/* Column where complementary information starts */

	/* Boundary 1 is the width required to display a complementary address
	   given that all immediate values are no more than 32 bits */
	int boundary1 = geom_addr + 29 + 8;
	/* Boudary 2 is the same, but also accepts 64-bit values */
	int boundary2 = geom_addr + 37 + 16;

	if(w < boundary1) geom_comp = -1;
	else if(w < boundary2) geom_comp = 3 + geom_addr + 27, geom_cmpl = 0;
	else geom_comp = 3 + geom_addr + 35, geom_cmpl = 1;

	printf("%d %d %d %d %d\n", geom_addr, geom_info, geom_base, geom_comp,
		geom_cmpl);
}
