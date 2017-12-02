//---
//	emu:breaks - breakpoint management
//
//	This module provides breakpoint management.
//---

#ifndef	BREAKS_H
#define	BREAKS_H

#include <stdint.h>

/*
	break_add() - create new breakpoints
	Add a breakpoint at the given address. It is not possible to add
	duplicate breakpoints; if one already exists, this function returns
	silently.

	@arg	address	Location of the new breakpoint
*/
void break_add(uint64_t address);

/*
	break_remove() - remove existing breakpoints
	Removes a breakpoint. An exception is raised if none is set at the
	given address.

	@arg	address	Location of the breakpoint to remove
*/
void break_remove(uint64_t address);

/*
	break_has() - check for breaks
	Checks if there is a breakpoint at the given address. Returns 1 if
	there's one, 0 otherwise.

	@arg	address	Where to look for a breakpoint
	@return		Whether the CPU should stop
*/
int break_has(uint64_t address);

/*
	break_show() - display all breakpoints to the console
	Shows the list of all set breakpoints on the debugger console.
*/
void break_show(void);

#endif	/* BREAKS_H */
