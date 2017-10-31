#define	_DEBUGGER_SOURCE
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <debugger.h>
#include <stdarg.h>

/* dbglog() -- print messages in the console */
void dbglog(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vw_printw(wcli, format, args);
	va_end(args);
}

/* dbgerr() -- print error messages */
void dbgerr(const char *format, ...)
{
	va_list args;

	wattron(wcli, COLOR_PAIR(color_error));
	dbglog("error: ");
	wattroff(wcli, COLOR_PAIR(color_error));

	va_start(args, format);
	vw_printw(wcli, format, args);
	va_end(args);
}

/* debugger_prompt() -- get a command, returning a static buffer */
char *debugger_prompt(void)
{
	static char cmd[80];

	/* TODO - Display a proper prompt */
	wattron(wcli, COLOR_PAIR(color_idle));
	dbglog("idle %% ");
	wattroff(wcli, COLOR_PAIR(color_idle));

	wgetnstr(wcli, cmd, 80);
	return cmd;
}
