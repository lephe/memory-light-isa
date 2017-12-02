#define	_DEBUGGER_SOURCE
#include <debugger.h>
#include <breaks.h>

#define	BREAK_MAX 100		/* Maximum number of breakpoints */
uint64_t bp[BREAK_MAX];		/* Addresses of breaks */
uint bpc = 0;			/* Number of breakpoints currently set */

/* break_add() - create new breakpoints */
void break_add(uint64_t address)
{
	if(break_has(address)) return;

	if(bpc >= BREAK_MAX)
	{
		dbgerr("maximum number of breakpoints reached\n");
		return;
	}

	bp[bpc++] = address;
}

/* break_remove() - remove existing breakpoints */
void break_remove(uint64_t address)
{
	uint i = 0;
	while(i < bpc && bp[i] != address) i++;
	if(i >= bpc)
	{
		dbgerr("breakpoint not set\n");
		return;
	}

	for(uint j = i + 1; j < bpc; j++) bp[j - 1] = bp[j];
	bpc--;
}

/* break_has() - check for breaks */
int break_has(uint64_t address)
{
	for(uint i = 0; i < bpc; i++) if(bp[i] == address) return 1;
	return 0;
}

/* break_show() - display all breakpoints to the console */
void break_show(void)
{
	if(!bpc)
	{
		dbglog("No breakpoints set.\n");
		return;
	}
	dbglog("Breakpoints currently set:\n");
	for(uint i = 0; i < bpc; i++) dbglog("%8x\n", bp[i]);
}
