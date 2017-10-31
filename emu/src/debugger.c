#define	_DEBUGGER_SOURCE
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>
#include <cpu.h>
#include <debugger.h>
#include <disasm.h>

/* Variables shared by the components of the debugger module */

WINDOW *wcode		= NULL;		/* Disassembled code */
WINDOW *wreg		= NULL;		/* Register state */
WINDOW *wstack		= NULL;		/* Stack view */
WINDOW *wframe		= NULL;		/* Console frame */
WINDOW *wcli		= NULL;		/* Debugger console */

cpu_t *debugger_cpu	= NULL;		/* Debugged CPU */
memory_t *debugger_mem	= NULL;		/* Debugged memory */



//---
//	Debugger functionalities
//
//	Most of them are called like programs from the command-line, thus the
//	argc/argv design. Some of them don't even read their arguments.
//
//	@arg	argc	Number of arguments, including command name
//	@arg	argv	NULL-terminated argument array
//---

static const char *help_string =
"Available commands:\n"
"  q    Quit debugger\n"
"  s    Step next instruction\n"
"  b    Manage breakpoints (try 'help b')\n"
"  r    Run until breakpoint, halt or end of program\n";
static const char *help_string_b =
"Manage breakpoints:\n"
"  b           Show all configured breakpoints\n"
"  b <addr>    Add a breakpoint at the given address\n"
"  b - <addr>  Remove a breakpoint at the given address\n";

/*
	debugger_help_log() -- simple hack to color the command names

	@arg	str	String to log; it will have command names colored
*/
static void debugger_help_log(const char *str)
{
	const char *next = NULL;

	while(*str)
	{
		next = strchr(str, '\n');
		if(next) next++;
		else next = str + strlen(str);

		if(*str != ' ')
		{
			dbgnlog(str, next - str);
			str = next;
			continue;
		}

		wattron(wcli, COLOR_PAIR(color_command));
		dbgnlog(str, 4);
		wattroff(wcli, COLOR_PAIR(color_command));
		dbgnlog(str + 4, next - str - 4);
		str = next;
	}
}

/*
	debugger_help()
	Provide help about a few predefined topics.
*/
static void debugger_help(int argc, char **argv)
{
	if(argc == 1) debugger_help_log(help_string);

	for(int i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "b")) debugger_help_log(help_string_b);
	}
}

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

/*
	debugger_free() -- free all debugger-allocated data
*/
static void debugger_free(void)
{
	if(wcode)	delwin(wcode);
	if(wreg)	delwin(wreg);
	if(wstack)	delwin(wstack);
	if(wframe)	delwin(wframe);
	if(wcli)	delwin(wcli);
}

/*
	draw_reg() -- draw the register state window
	TODO - This probably needs to be moved into a separate component
*/
static void draw_reg(void)
{
	cpu_t *cpu = debugger_cpu;	/* Naming shorthand */

	/* Border and title */

	wclear(wreg);
	wborder(wreg, 0, ' ', 0, ' ', ACS_TTEE, ACS_HLINE, ACS_VLINE, ' ');

	wattron(wreg, A_BOLD);
	mvwaddstr(wreg, 0, 1, " Register info ");
	wattroff(wreg, A_BOLD);

	/* Memory pointers */

	const char *ptrs[] = { "PC", "PR", "A0", "A1" };
	for(size_t i = 0; i < 4; i++)
		mvwprintw(wreg, 2 + i, 3, "%s  %08x", ptrs[i], cpu->ptr[i]);

	/* General-purpose registers */

	for(size_t i = 0; i < 8; i++)
		mvwprintw(wreg, 7 + i, 3, "r%d  %016lx", i, cpu->r[i]);

	/* Processor flags */

	mvwprintw(wreg, 16, 3, "Flags:");
	mvwprintw(wreg, 17, 3, "Z=%d C=%d N=%d", cpu->z, cpu->c, cpu->z);

	wrefresh(wreg);
}

/*
	draw_stack() -- draw the stack state window
	TODO - This probably needs to be moved into a separate component
*/
static void draw_stack(void)
{
	/* Border and title */

	wclear(wstack);
	wborder(wstack, 0, 0, 0, ' ', ACS_TTEE, 0, ACS_VLINE, ACS_VLINE);

	wattron(wstack, A_BOLD);
	mvwaddstr(wstack, 0, 1, " Stack state ");
	wattroff(wstack, A_BOLD);

	/* Some colors, since there's nothing on the stack for now */

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

/*
	draw_frame() -- draw the frame around the console
*/
static void draw_frame(void)
{
	/* Border and title, again */

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

	/* We leave the rest to the console proper. And to make sure the frame
	   is not drawn on top of the console, we also refresh the console */
	wrefresh(wcli);
}

/* debugger() -- run the text interface debugger */
void debugger(const char *filename, cpu_t *cpu)
{
	/* Initialize the main screen */
	initscr();
	start_color();

	/* Set up some color pairs, all on black background.
	   (This may not *actually* be required, but I'd rather make sure by
	   explicitly doing it than relying on default behavior) */
	init_pair(color_black,	COLOR_BLACK,	COLOR_BLACK);
	init_pair(color_red,	COLOR_RED,	COLOR_BLACK);
	init_pair(color_green,	COLOR_GREEN,	COLOR_BLACK);
	init_pair(color_yellow,	COLOR_YELLOW,	COLOR_BLACK);
	init_pair(color_blue,	COLOR_BLUE,	COLOR_BLACK);
	init_pair(color_magenta,COLOR_MAGENTA,	COLOR_BLACK);
	init_pair(color_cyan,	COLOR_CYAN,	COLOR_BLACK);
	init_pair(color_white,	COLOR_WHITE,	COLOR_BLACK);

	/* Separate the screen into four panes */
	size_t h, w;
	getmaxyx(stdscr, h, w);

	size_t w3 = 27;
	size_t w2 = 26;
	size_t w1 = w - w2 - w3;
	size_t h1 = 6 * h / 10, h2 = h - h1;

	/* Allocate the five windows, the frame one containing the cli one.
	   This design allows the debugger ti have a frame around its console
	   and to use the default functions of ncurses to manage the console.
	   (Another buffer of the same size would have been required anyway) */

	/* TODO - Use stdscr for the cli frame (and maybe all others too?) */
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

	/* Set up the debugger */

	debugger_cpu = cpu;
	debugger_mem = cpu->mem;

	debugger_code_init();

	/* Authorize ncurses to scroll the console */
	scrollok(wcli, 1);
	wmove(wcli, 0, 0);

	/* Draw initial things at startup */
	debugger_code();
	draw_reg();
	draw_stack();
	draw_frame();

	/* Greet the user (it's important to greet the user) */
	if(filename)
	{
		wattron(wcli, COLOR_PAIR(color_magenta));
		dbglog("Initial load: %s\n", filename);
		wattroff(wcli, COLOR_PAIR(color_magenta));
	}
	dbglog("Welcome to the debugger. Type 'help' to get a summary of the "
		"commands.\n");

	/* Now, finally run the debugger! */

	char *cmd, **argv;
	int argc;

	while(1)
	{
		/* Get a string, and find out the command name */
		cmd = debugger_prompt(&argv);
		argv[0] = strtok(cmd, " \t");
		if(!argv[0]) continue;

		/* Get the command arguments */
		argc = 1;
		while((argv[argc] = strtok(NULL, " \t"))) argc++;

		/* Call the associated function */
		if(!strcmp(argv[0], "q")) break;
		else if(!strcmp(argv[0], "help")) debugger_help(argc, argv);
		else dbgerr("unknown command '%s'\n", argv[0]);
	}

	endwin();
	debugger_free();
}
