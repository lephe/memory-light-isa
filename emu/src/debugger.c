#define	_DEBUGGER_SOURCE
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>
#include <cpu.h>
#include <debugger.h>
#include <disasm.h>
#include <breaks.h>

/* Variables shared by the components of the debugger module */

WINDOW *wcode		= NULL;		/* Disassembled code */
WINDOW *wreg		= NULL;		/* Register state */
WINDOW *wmem		= NULL;		/* Stack view */
WINDOW *wframe		= NULL;		/* Console frame */
WINDOW *wcli		= NULL;		/* Debugger console */

cpu_t *debugger_cpu	= NULL;		/* Debugged CPU */
memory_t *debugger_mem	= NULL;		/* Debugged memory */

/* Execution state */
debugger_state_t debugger_state = state_idle;

static void draw_reg(void);


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
"  s <n>       Step <n> instructions (default 1)\n"
"  b           Manage breakpoints (try 'help b')\n"
"  r           Run until breakpoint, halt or end of program\n"
"  d           Disassemble program (try 'help d')\n"
"  m <addr>    Display memory at address <addr> (hexadecimal, without '0x')\n"
"    :ptr      Display memory at given pointer (:pc, :sp, :a0, :a1)\n"
"  c           Show counts of executed instructions so far\n"
"  q           Quit debugger\n";
static const char *help_string_b =
"Manage breakpoints:\n"
"  b           Show all configured breakpoints\n"
"  b <addr>    Add a breakpoint at the given address\n"
"  b - <addr>  Remove a breakpoint at the given address\n";
static const char *help_string_d =
"Disassemble program:\n"
"  d           Disassemble at PC and follow PC during execution\n"
"  d <addr>    Disassemble the given location and stay there when PC moves\n";

/* TODO - Show stack, change program state, more? */

/*
	cmd_help_color() -- simple hack to color the command names

	@arg	str	String to color; it will have command names colored
*/
static void cmd_help_color(const char *str)
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
	cmd_help()
	Provide help about a few predefined topics.
*/
static void cmd_help(int argc, __attribute__((unused)) char **argv)
{
	if(argc == 1) cmd_help_color(help_string);

	for(int i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "b")) cmd_help_color(help_string_b);
		if(!strcmp(argv[i], "d")) cmd_help_color(help_string_d);
	}
}

static void cmd_run_cpu(int steps)
{
	cpu_t *cpu = debugger_cpu;
	cpu->h = cpu->m = cpu->t = 0;

	/* Switch to idle mode "by default" */
	debugger_state = state_idle;

	/* Also check when CPU reaches the end of the code */
	while(cpu->ptr[PC] < debugger_mem->text && steps)
	{
		cpu_execute(cpu);
		if(steps > 0) steps--;
		if(cpu->h) break;

		/* Also break if a breakpoint is reached */
		if(break_has(cpu->ptr[PC]))
		{
			debugger_state = state_break;
			break;
		}
	}

	/* FIXME - We don't need to refresh the code panel now if
	   1. We are not following PC
	   2. PC was not, and is still not, visible in the current area */
	debugger_code();

	draw_reg();

	/* FIXME - We don't need to refresh the memory panel if the memory area
	   that was written to is not visible */
	if(cpu->m) debugger_memory();

	if(cpu->h) debugger_state = state_halt;
}

static void cmd_step(int argc, char **argv)
{
	int steps = (argc >= 2) ? atoi(argv[1]) : 1;
	cmd_run_cpu(steps);
}

static void cmd_run(void)
{
	cmd_run_cpu(-1);
}

static void cmd_disasm(int argc, char **argv)
{
	if(argc == 1)
	{
		debugger_code_mode(1, 0);
		debugger_code();
		return;
	}

	if(argc > 3)
	{
		dbgerr("d: too much arguments (see 'help d')\n");
		return;
	}

	uint64_t address;
	int x = sscanf(argv[1], "%lx", &address);
	if(x < 1)
	{
		dbgerr("d: invalid program address\n");
		return;
	}

	debugger_code_mode(0, address);
	debugger_code();
}

static void cmd_break(int argc, char **argv)
{
	if(argc == 1)
	{
		break_show();
		return;
	}
	if(argc > 3)
	{
		dbgerr("b: too much arguments (see 'help b')\n");
		return;
	}
	if(argc == 3 && strcmp(argv[1], "-"))
	{
		dbgerr("b: invalid syntax (see 'help b')\n");
		return;
	}

	uint64_t address;
	int x = sscanf(argv[argc - 1], "%lx", &address);
	if(x < 1)
	{
		dbgerr("b: invalid breakpoint address\n");
		return;
	}

	(argc == 2) ? break_add(address) : break_remove(address);
	/* Adding breakpoints usually change the color of addresses */
	debugger_code();
}

static void cmd_mem(int argc, char **argv)
{
	if(argc < 2)
	{
		dbgerr("m: address missing (see 'help')\n");
		return;
	}

	uint64_t address;

	if(argv[1][0] == ':')
	{
		const char *names[4] = { ":pc", ":sp", ":a0", ":a1" };
		int i;
		for(i = 0; i < 4; i++) if(!strcmp(argv[1], names[i]))
		{
			address = debugger_cpu->ptr[i];
			break;
		}
		if(i >= 4)
		{
			dbgerr("m: unknown pointer name (see 'help')\n");
			return;
		}
	}
	else address = strtol(argv[1], NULL, 16);

	if(address >= debugger_mem->memsize)
	{
		dbgerr("m: out of bounds (%lx > %lx)\n", address,
			debugger_mem->memsize);
		return;
	}
	debugger_memory_move(address);
	debugger_memory();
}

static void cmd_counts(void)
{
	size_t *counts = cpu_counts();

	for(uint i = 0; i < DISASM_INS_COUNT; i++)
	{
		const char *format = disasm_format(i);
		dbglog("  %6s %-6u", format + 6, counts[i]);
		if((i & 7) == 7) dbglog("\n");
	}

	if((DISASM_INS_COUNT & 7) != 7) dbglog("\n");
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
	if(wmem)	delwin(wmem);
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

	const char *ptrs[] = { "PC", "SP", "A0", "A1" };
	for(size_t i = 0; i < 4; i++)
		mvwprintw(wreg, 2 + i, 3, "%s  %08x", ptrs[i], cpu->ptr[i]);

	/* General-purpose registers */

	for(size_t i = 0; i < 8; i++)
		mvwprintw(wreg, 7 + i, 3, "r%d  %016lx", i, cpu->r[i]);

	/* Processor flags */

	mvwprintw(wreg, 16, 3, "Flags:");
	mvwprintw(wreg, 17, 3, "Z=%d N=%d C=%d V=%d",
		cpu->z, cpu->n, cpu->c, cpu->v);

	wrefresh(wreg);
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
	getbegyx(wmem, y, x);
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

	size_t w3 = 35;
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
	wmem	= newwin(h1, w3, 0, w1 + w2);
	wframe	= newwin(h2, w, h1, 0);
	wcli	= newwin(h2 - 2, w - 2, h1 + 1, 1);

	if(!wcode || !wreg || !wmem || !wframe || !wcli)
	{
		debugger_free();
		return;
	}

	/* Set up the debugger */

	debugger_cpu = cpu;
	debugger_mem = cpu->mem;

	debugger_code_init();
	debugger_memory_move(0x000000);

	/* Authorize ncurses to scroll the console */
	scrollok(wcli, 1);
	wmove(wcli, 0, 0);

	/* Draw initial things at startup */
	debugger_code();
	draw_reg();
	debugger_memory();
	draw_frame();

	/* Greet the user (it's important to greet the user) */
	if(filename)
	{
		wattron(wcli, COLOR_PAIR(color_magenta));
		dbglog("Loaded %s\n", filename);
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
		if(!strcmp(argv[0], "help")) cmd_help(argc, argv);
		else if(!strcmp(argv[0], "q")) break;
		else if(!strcmp(argv[0], "s")) cmd_step(argc, argv);
		else if(!strcmp(argv[0], "r")) cmd_run();
		else if(!strcmp(argv[0], "d")) cmd_disasm(argc, argv);
		else if(!strcmp(argv[0], "b")) cmd_break(argc, argv);
		else if(!strcmp(argv[0], "m")) cmd_mem(argc, argv);
		else if(!strcmp(argv[0], "c")) cmd_counts();
		else dbgerr("unknown command '%s'\n", argv[0]);
	}

	debugger_free();

	/* This will not actually free the screen, which is normal for an
	   ncurses implementation. Memory leaks related to initscr() are
	   entirely normal */
	endwin();
}
