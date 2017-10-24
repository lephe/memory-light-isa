#include <ncurses.h>
#include <cpu.h>
#include <debugger.h>

/* debug() -- run the text interface debugger */
void debug(cpu_t *cpu)
{
	initscr();
	printw("Hello, World!");
	refresh();
	getch();
	endwin();
}
