#include <stdlib.h>
#include <string.h>

#define	_DEBUGGER_SOURCE
#include <ncurses.h>
#include <cpu.h>
#include <debugger.h>
#include <disasm.h>

static void draw_reg(void);
static void draw_stack(void);
static void draw_frame(void);

WINDOW *wcode		= NULL;		/* Disassembled code */
WINDOW *wreg		= NULL;		/* Register state */
WINDOW *wstack		= NULL;		/* Stack view */
WINDOW *wframe		= NULL;		/* Console frame */
WINDOW *wcli		= NULL;		/* Debugger console */

cpu_t *debugger_cpu	= NULL;		/* Debugged CPU */
memory_t *debugger_mem	= NULL;		/* Debugged memory */

//---
//	Debugger functions
//---

static const char *help_string =
"Available commands:\n"
"  q         Quit debugger\n"
"  s         Step next instruction\n"
"  b <addr>  Set breakpoint at given address\n"
"  r         Run until breakpoint, halt or end of program\n"
;

static void debugger_step(void)
{
}

static void debugger_break(void)
{
}

static void debugger_run(void)
{
}

//---
//	Debugger program
//---

/* debugger_free() -- free all text interface data */
void debugger_free(void)
{
	if(wcode)	delwin(wcode);
	if(wreg)	delwin(wreg);
	if(wstack)	delwin(wstack);
	if(wframe)	delwin(wframe);
	if(wcli)	delwin(wcli);
}

/* debugger() -- run the text interface debugger */
void debugger(const char *filename, cpu_t *cpu)
{
	/* Initialize the main screen */
	initscr();
	start_color();

	/* Set up some color pairs, all on black background */
	init_pair(color_black,	COLOR_BLACK,	COLOR_BLACK);
	init_pair(color_red,	COLOR_RED,	COLOR_BLACK);
	init_pair(color_green,	COLOR_GREEN,	COLOR_BLACK);
	init_pair(color_yellow,	COLOR_YELLOW,	COLOR_BLACK);
	init_pair(color_blue,	COLOR_BLUE,	COLOR_BLACK);
	init_pair(color_magenta,COLOR_MAGENTA,	COLOR_BLACK);
	init_pair(color_cyan,	COLOR_CYAN,	COLOR_BLACK);
	init_pair(color_white,	COLOR_WHITE,	COLOR_BLACK);
	/* Do the same on light background */
	init_pair(8 | color_black,	COLOR_BLACK,	COLOR_WHITE);
	init_pair(8 | color_red,	COLOR_RED,	COLOR_WHITE);
	init_pair(8 | color_green,	COLOR_GREEN,	COLOR_WHITE);
	init_pair(8 | color_yellow,	COLOR_YELLOW,	COLOR_WHITE);
	init_pair(8 | color_blue,	COLOR_BLUE,	COLOR_WHITE);
	init_pair(8 | color_magenta,	COLOR_MAGENTA,	COLOR_WHITE);
	init_pair(8 | color_cyan,	COLOR_CYAN,	COLOR_WHITE);
	init_pair(8 | color_white,	COLOR_BLACK,	COLOR_WHITE);

	/* Immediately separate it into four panes */
	size_t h, w;
	getmaxyx(stdscr, h, w);

	/* Allocate the four windows */

	size_t w3 = 27;
	size_t w2 = 26;
	size_t w1 = w - w2 - w3;
	size_t h1 = 6 * h / 10, h2 = h - h1;

	wcode	= newwin(h1, w1, 0, 0);
	wreg	= newwin(h1, w2, 0, w1);
	wstack	= newwin(h1, w3, 0, w1 + w2);
	wframe	= newwin(h2, w, h1, 0);
	wcli	= newwin(h2 - 2, w - 2, h1 + 1, 1);

	if(!wcode || !wreg || !wstack || !wframe || !wcli)
	{
		debugger_free();
		return;
	}

	debugger_cpu = cpu;
	debugger_mem = cpu->mem;

	debugger_code_init();
	debugger_code();

	wmove(wcli, 0, 0);
	scrollok(wcli, 1);

	draw_reg();
	draw_stack();
	draw_frame();

	/* TODO - Use a proper console to do this */

	/* TODO - In magenta */
	wattron(wcli, COLOR_PAIR(color_magenta));
	dbglog("Initial load: %s\n", filename);
	wattroff(wcli, COLOR_PAIR(color_magenta));
	dbglog("Welcome to the debuger. Type 'help' to get a summary of the "
		"commands.\n");


	while(1)
	{
		char *cmd = debugger_prompt();
		if(!cmd[0]) continue;

		/* Locate command base name */
		if(!strcmp(cmd, "help")) dbglog(help_string);
		else if(!strcmp(cmd, "q")) break;
		else dbgerr("unknown command '%s'\n", cmd);
	}

	endwin();
	debugger_free();
}

static void draw_reg(void)
{
	cpu_t *cpu = debugger_cpu;	/* Naming shorthand */

	wclear(wreg);
	wborder(wreg, 0, ' ', 0, ' ', ACS_TTEE, ACS_HLINE, ACS_VLINE, ' ');

	wattron(wreg, A_BOLD);
	mvwaddstr(wreg, 0, 1, " Register info ");
	wattroff(wreg, A_BOLD);

	const char *ptrs[] = { "PC", "PR", "A0", "A1" };
	for(size_t i = 0; i < 4; i++)
		mvwprintw(wreg, 2 + i, 3, "%s  %08x", ptrs[i], cpu->ptr[i]);

	for(size_t i = 0; i < 8; i++)
		mvwprintw(wreg, 7 + i, 3, "r%d  %016lx", i, cpu->r[i]);

	mvwprintw(wreg, 16, 3, "Flags:");
	mvwprintw(wreg, 17, 3, "Z=%d C=%d N=%d", cpu->z, cpu->c, cpu->z);

	wrefresh(wreg);
}

static void draw_stack(void)
{
	wclear(wstack);
	wborder(wstack, 0, 0, 0, ' ', ACS_TTEE, 0, ACS_VLINE, ACS_VLINE);

	wattron(wstack, A_BOLD);
	mvwaddstr(wstack, 0, 1, " Stack state ");
	wattroff(wstack, A_BOLD);

	mvwaddstr(wstack, 2, 3, "Color set:");
	for(int i = 0; i < 8; i++)
	{
		wmove(wstack, i + 3, 3);
		wattron(wstack, COLOR_PAIR(i));
		waddstr(wstack, "color  ");
		wattron(wstack, A_DIM);
		waddstr(wstack, "color  ");
		wattroff(wstack, COLOR_PAIR(i) | A_DIM);
	}

	wrefresh(wstack);
}

static void draw_frame(void)
{
	wclear(wframe);
	wborder(wframe, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE, 0, 0);

	/* Little trick to show the proper tees */
	size_t x, __attribute__((unused)) y;
	getbegyx(wreg, y, x);
	mvwaddch(wframe, 0, x, ACS_BTEE);
	getbegyx(wstack, y, x);
	mvwaddch(wframe, 0, x, ACS_BTEE);

	wattron(wframe, A_BOLD);
	mvwaddstr(wframe, 0, 1, " Command line ");
	wattroff(wframe, A_BOLD);
	wrefresh(wframe);

	/* Little trick to show the console on top */
	wrefresh(wcli);
}
