#include <ncurses.h>
#include <cpu.h>
#include <debugger.h>

static void draw_code(void);
static void draw_reg(void);
static void draw_stack(void);
static void draw_cli(void);

WINDOW *wcode	= NULL;		/* Disassembled code */
WINDOW *wreg	= NULL;		/* Register state */
WINDOW *wstack	= NULL;		/* Stack view */
WINDOW *wcli	= NULL;		/* Debugger console */

/* debug_free() -- free all text interface data */
void debug_free(void)
{
	if(wcode)	delwin(wcode);
	if(wreg)	delwin(wreg);
	if(wstack)	delwin(wstack);
	if(wcli)	delwin(wcli);
}

/* debug() -- run the text interface debugger */
void debug(cpu_t *cpu)
{
	/* Initialize the main screen */
	initscr();

	/* Immediately separate it into four panes */
	size_t w, h;
	getmaxyx(stdscr, h, w);

	size_t w1 = w - 2 * (w / 3), w2 = w / 3, w3 = w / 3;
	size_t h1 = 7 * h / 10, h2 = h - h1;

	wcode	= newwin(h1, w1, 0, 0);
	wreg	= newwin(h1, w2, 0, w1);
	wstack	= newwin(h1, w3, 0, w1 + w2);
	wcli	= newwin(h2, w, h1, 0);

	if(!wcode || !wreg || !wstack || !wcli)
	{
		debug_free();
		return;
	}

	draw_code();
	draw_reg();
	draw_stack();
	draw_cli();

	char str[80];
	mvwprintw(wcli, 1, 1, "Welcome to the debugger. Type 'help' to get a "
		"summary of the commands.");
	mvwprintw(wcli, 2, 1, "(TODO. Actually <Enter> will quit)");
	mvwprintw(wcli, 3, 1, "idle %% ");
	wgetnstr(wcli, str, 80);
	endwin();
}

static void draw_code(void)
{
	wclear(wcode);
	wborder(wcode, 0, ' ', 0, ' ', 0, ACS_HLINE, ACS_VLINE, ' ');

	wattron(wcode, A_BOLD);
	mvwaddstr(wcode, 0, 1, " Disassembled code ");
	wattroff(wcode, A_BOLD);
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
