#include <stdlib.h>
#include <string.h>

#include <ncurses.h>
#include <cpu.h>
#include <debugger.h>
#include <disasm.h>

static void draw_code(void);
static void draw_reg(void);
static void draw_stack(void);
static void draw_cli(void);

static WINDOW *wcode	= NULL;		/* Disassembled code */
static WINDOW *wreg	= NULL;		/* Register state */
static WINDOW *wstack	= NULL;		/* Stack view */
static WINDOW *wcli	= NULL;		/* Debugger console */

static cpu_t *cpu	= NULL;		/* Debugged CPU */
static memory_t *mem	= NULL;		/* Debugged memory */

static char *disasm	= NULL;		/* Disassembler instruction array */
static size_t disasm_s	= 0;		/* Size of this array */

/* Some color constants, and some others with semantic names */
enum
{
	color_black	= 0,
	color_red	= 1,
	color_green	= 2,
	color_yellow	= 3,
	color_blue	= 4,
	color_magenta	= 5,
	color_cyan	= 6,
	color_white	= 7,

	/* Console */
	color_error	= color_red,
	color_warn	= color_yellow,

	/* Disassembler */
	color_arithm	= color_white,
	color_test	= color_white,
	color_let	= color_white,
	color_jump	= color_cyan,
	color_memory	= color_yellow,
	color_control	= color_magenta,
};

/* debugger_free() -- free all text interface data */
void debugger_free(void)
{
	if(wcode)	delwin(wcode);
	if(wreg)	delwin(wreg);
	if(wstack)	delwin(wstack);
	if(wcli)	delwin(wcli);
	if(disasm)	free(disasm);
}

static void debugger_arg(uint32_t *ptr, char **str, int type)
{
	const char *ptr_names[4] = { "pc", "sp", "a0", "a1" };
	const char *sign;
	uint gen, size;
	int64_t i64;
	uint64_t u64;

	switch(type)
	{
	case arg_none: return;
	case arg_reg:
		*str += sprintf(*str, " r%d", disasm_reg(mem, ptr));
		return;
	case arg_dir:
		gen = disasm_dir(mem, ptr);
		*str += sprintf(*str, " %s", gen ? "right" : "left");
		return;
	case arg_cond:
		*str += sprintf(*str, " c%d", disasm_dir(mem, ptr));
		return;
	case arg_addr:
		i64 = disasm_addr(mem, ptr, &size);
		if(i64 < 0) sign = "-", i64 = -i64;
		else sign = "";
		*str += sprintf(*str, " %s%0*lx", sign, size >> 2, i64);
		return;
	case arg_lconst:
		u64 = disasm_lconst(mem, ptr, &size);
		*str += sprintf(*str, " %0*lx", size >> 2, u64);
		return;
	case arg_aconst:
		i64 = disasm_aconst(mem, ptr, &size);
		if(i64 < 0) sign = "-", i64 = -i64;
		else sign = "";
		*str += sprintf(*str, " %s%0*lx", sign, size >> 2, i64);
		return;
	case arg_shift:
		*str += sprintf(*str, " %d", disasm_shift(mem, ptr));
		return;
	case arg_size:
		*str += sprintf(*str, " %d", disasm_size(mem, ptr));
		return;
	case arg_pointer:
		gen = disasm_pointer(mem, ptr);
		*str += sprintf(*str, " %s", ptr_names[gen]);
		return;
	}
}

static void debugger_disasm(void)
{
	uint32_t ptr = cpu->ptr[PC];
	const char *format;

	for(size_t i = 0; i < disasm_s; i++)
	{
		char *str = disasm + 43 * i;

		/* We check ptr + 7 because there might be up to 7 bits padding
		  in the source binary and no instruction fits on 7 bits */
		if(ptr + 7 >= mem->text)
		{
			*str = 0;
			continue;
		}

		uint32_t copy = ptr;
		disasm_opcode(mem, &ptr, &format);

		str += sprintf(str, "%08x:  %-8s ", copy, format + 4);

		for(size_t j = 0; j < 3; j++)
			debugger_arg(&ptr, &str, format[j]);
	}
}

/* debugger() -- run the text interface debugger */
void debugger(const char *filename, cpu_t *arg_cpu)
{
	/* Initialize the main screen */
	initscr();
	start_color();

	/* Set up some color pairs, all on black background */
	init_pair(color_red,	COLOR_RED,	COLOR_BLACK);
	init_pair(color_green,	COLOR_GREEN,	COLOR_BLACK);
	init_pair(color_yellow,	COLOR_YELLOW,	COLOR_BLACK);
	init_pair(color_blue,	COLOR_BLUE,	COLOR_BLACK);
	init_pair(color_magenta,COLOR_MAGENTA,	COLOR_BLACK);
	init_pair(color_cyan,	COLOR_CYAN,	COLOR_BLACK);
	init_pair(color_white,	COLOR_WHITE,	COLOR_BLACK);

	/* Immediately separate it into four panes */
	size_t w, h;
	getmaxyx(stdscr, h, w);

	/* Allocate the four windows */

	size_t w1 = w - 2 * (w / 3), w2 = w / 3, w3 = w / 3;
	size_t h1 = 6 * h / 10, h2 = h - h1;

	wcode	= newwin(h1, w1, 0, 0);
	wreg	= newwin(h1, w2, 0, w1);
	wstack	= newwin(h1, w3, 0, w1 + w2);
	wcli	= newwin(h2, w, h1, 0);

	/* Allocate the disassembler instruction buffer, lines of 34 chars */

	disasm	= calloc(h1 - 3, 43);
	disasm_s = h1 - 3;

	if(!wcode || !wreg || !wstack || !wcli || !disasm)
	{
		debugger_free();
		return;
	}

	cpu = arg_cpu;
	mem = arg_cpu->mem;

	debugger_disasm();

	draw_code();
	draw_reg();
	draw_stack();
	draw_cli();

	/* TODO - Use a proper console to do this */
	char str[10];
	mvwprintw(wcli, 1, 1, "Welcome to the debugger. Type 'help' to get a "
		"summary of the commands.");
	mvwprintw(wcli, 2, 1, "(TODO. Actually <Enter> will quit)");
	wattron(wcli, COLOR_PAIR(color_yellow));
	mvwprintw(wcli, 3, 1, "idle %% ");
	wattroff(wcli, COLOR_PAIR(color_yellow));
	wgetnstr(wcli, str, 10);

	endwin();
	debugger_free();
}

static void draw_code(void)
{
	wclear(wcode);
	wborder(wcode, 0, ' ', 0, ' ', 0, ACS_HLINE, ACS_VLINE, ' ');

	wattron(wcode, A_BOLD);
	mvwaddstr(wcode, 0, 1, " Disassembled code ");
	wattroff(wcode, A_BOLD);

	for(size_t i = 0; i < disasm_s; i++)
	{
		const char *str = disasm + 43 * i;

		wattron(wcode, A_DIM | COLOR_PAIR(color_white));
		mvwaddnstr(wcode, i + 2, 3, str, 11);
		wattroff(wcode, A_DIM | COLOR_PAIR(color_white));

		/* Setting the proper color for the next instruction */
		int color = color_white;
		if(str[11] == 'A') color = color_arithm;
		if(str[11] == 'T') color = color_test;
		if(str[11] == 'L') color = color_let;
		if(str[11] == 'J') color = color_jump;
		if(str[11] == 'M') color = color_memory;
		if(str[11] == 'C') color = color_control;

		str += 13;
		wattron(wcode, COLOR_PAIR(color));
		waddnstr(wcode, str, 8);

		str += 8;
		wattroff(wcode, COLOR_PAIR(color));
		waddstr(wcode, str);
	}

	wrefresh(wcode);
}

static void draw_reg(void)
{
	wclear(wreg);
	wborder(wreg, 0, ' ', 0, ' ', ACS_TTEE, ACS_HLINE, ACS_VLINE, ' ');

	wattron(wreg, A_BOLD);
	mvwaddstr(wreg, 0, 1, " Register info ");
	wattroff(wreg, A_BOLD);
	wrefresh(wreg);
}

static void draw_stack(void)
{
	wclear(wstack);
	wborder(wstack, 0, 0, 0, ' ', ACS_TTEE, 0, ACS_VLINE, ACS_VLINE);

	wattron(wstack, A_BOLD);
	mvwaddstr(wstack, 0, 1, " Stack state ");
	wattroff(wstack, A_BOLD);
	wrefresh(wstack);
}

static void draw_cli(void)
{
	wclear(wcli);
	wborder(wcli, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE, 0, 0);

	/* Little trick to show the proper tees */
	size_t x, __attribute__((unused)) y;
	getbegyx(wreg, y, x);
	mvwaddch(wcli, 0, x, ACS_BTEE);
	getbegyx(wstack, y, x);
	mvwaddch(wcli, 0, x, ACS_BTEE);

	wattron(wcli, A_BOLD);
	mvwaddstr(wcli, 0, 1, " Command line ");
	wattroff(wcli, A_BOLD);
	wrefresh(wcli);
}
