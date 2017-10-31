#ifndef	DEBUGGER_H
#define	DEBUGGER_H

#include <cpu.h>
#include <memory.h>

/* debugger() -- run the text interface debugger */
void debugger(const char *filename, cpu_t *cpu);

//---
//	Internal definitions for other files from the debugger module
//---

#ifdef	_DEBUGGER_SOURCE

/* Ncurses window panels */
extern WINDOW *wcode;
extern WINDOW *wreg;
extern WINDOW *wstack;
extern WINDOW *wframe;
extern WINDOW *wcli;

/* CPU and memory being used for emulation */
extern cpu_t *debugger_cpu;
extern memory_t *debugger_mem;

/* debugger_code_init() -- compute the geometry of the disassembled output */
void debugger_code_init(void);
/* debugger_code() -- display disassembled code in the wcode panel */
void debugger_code(void);

/* debugger_prompt() -- get a command, returning a static buffer */
char *debugger_prompt(void);
/* dbglog() -- print messages in the console */
void dbglog(const char *format, ...);
/* dbgerr() -- print error messages */
void dbgerr(const char *format, ...);

/* Some color constants, and some others with semantic names */
typedef enum
{
	color_black	= 0,
	color_red	= 1,
	color_green	= 2,
	color_yellow	= 3,
	color_blue	= 4,
	color_magenta	= 5,
	color_cyan	= 6,
	color_white	= 7,

	/* Some values used here include 8-15 with light background... */

	/* Console */
	color_error	= color_red,
	color_idle	= color_yellow,

	/* Disassembler */
	color_arithm	= color_white,
	color_test	= color_white,
	color_let	= color_green,
	color_jump	= color_cyan,
	color_memory	= color_red,
	color_control	= color_magenta,

} debugger_color_t;

#endif	/* _DEBUGGER_SOURCE */
#endif	/* DEBUGGER_H */
