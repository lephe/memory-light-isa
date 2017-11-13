//---
//	emu:debugger:memory - show memory
//
//	This module is an interface component of the debugger; it show the
//	memory in the memory view panel when the user requests it
//---

#define	_DEBUGGER_SOURCE
#include <ncurses.h>
#include <debugger.h>
#include <memory.h>

/* Address of the memory section currently displayed in the panel */
static uint64_t address = 0x000000;

/* debugger_memory_move() -- display another section of the memory */
void debugger_memory_move(uint64_t address_arg)
{
	address = address_arg;
	debugger_memory();
}

/* debugger_memory() -- refresh the memory panel */
void debugger_memory(void)
{
	size_t h, __attribute__((unused)) w;
	getmaxyx(wcode, h, w);

	wclear(wmem);
	wborder(wmem, 0, 0, 0, ' ', ACS_TTEE, 0, ACS_VLINE, ACS_VLINE);

	wattron(wmem, A_BOLD);
	mvwaddstr(wmem, 0, 1, " Memory view ");
	wattroff(wmem, A_BOLD);

	uint64_t src = address;
	uint64_t data;
	size_t row;

	for(row = 2; row < h - 1; row++)
	{
		if(src + 64 > debugger_mem->memsize) break;

		mvwprintw(wmem, row, 3, "%08x", src);

		data = memory_read(debugger_mem, src, 64);
		wprintw(wmem, "  %04x %04x %04x %04x",
			data >> 48, (data >> 32) & 0xffff,
			(data >> 16) & 0xffff, data & 0xffff);

		src += 64;
	}

	wrefresh(wmem);
}
