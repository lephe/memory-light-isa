//---
//	emu:debugger:console - manage the console interface with the user
//
//	This module is an interface-oriented component of the debugger. It
//	manages the command-line window both for command input and data output.
//---

#define	_DEBUGGER_SOURCE
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <debugger.h>
#include <stdarg.h>

/* dbglog() -- print messages to the console */
void dbglog(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vw_printw(wcli, format, args);
	va_end(args);
}

/* dbgnlog() -- print partial messages to the console */
void dbgnlog(const char *str, size_t n)
{
	waddnstr(wcli, str, n);
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
char *debugger_prompt(char ***args_ptr)
{
	static char cmd[80];	/* Array that holds the command string */
	static char *args[41];	/* Array for the arguments - no more than 41 */

	switch(debugger_state)
	{
	case state_idle:
		wattron(wcli, COLOR_PAIR(color_idle));
		dbglog("idle %% ");
		wattroff(wcli, COLOR_PAIR(color_idle));
		break;
	case state_break:
		wattron(wcli, COLOR_PAIR(color_break));
		dbglog("break%% ");
		wattroff(wcli, COLOR_PAIR(color_break));
		break;
	case state_halt:
	default:
		wattron(wcli, COLOR_PAIR(color_halt));
		dbglog("halt %% ");
		wattroff(wcli, COLOR_PAIR(color_halt));
		break;
	}

	/* Get the command, and provide the args array if requested */
	wgetnstr(wcli, cmd, 80);
	if(args_ptr) *args_ptr = args;
	return cmd;
}
