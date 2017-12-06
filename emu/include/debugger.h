//---
//	emu:debugger - graphical debugger interface
//
//	This module provides a text interface to visualize what happens inside
//	the CPU, as well as monitoring and debugging tools. See the other files
//	from the debugger category for more detail.
//---

#ifndef	DEBUGGER_H
#define	DEBUGGER_H

#include <cpu.h>
#include <memory.h>
#include <ncurses.h>

/*
	debugger() -- run the text interface debugger
	The debugger takes control of the execution flow of the emulated
	program. Returns when the user sends the leave command.

	@arg	filename	Name of the file loaded in the memory, if any
	@arg	cpu		CPU to monitor
*/
void debugger(const char *filename, cpu_t *cpu);



//---
//	Internal definitions for other files from the debugger module
//---

/* This macro exposes definitions that should only be seen by the components of
   the debugger module */
#ifdef	_DEBUGGER_SOURCE

/* Ncurses window panels */
extern WINDOW *wcode;
extern WINDOW *wreg;
extern WINDOW *wmem;
extern WINDOW *wframe;
extern WINDOW *wcli;

/* CPU and memory being used for emulation */
extern cpu_t *debugger_cpu;
extern memory_t *debugger_mem;

/*
	debugger_state_t enumeration
	Different states of execution for the debugged program.
*/
typedef enum
{
	state_idle	= 0,	/* Program is ready to run */
	state_break	= 1,	/* Program has reached breakpoint */
	state_halt	= 2,	/* Program has reached end or infinite loop */

} debugger_state_t;

/* Current execution state */
extern debugger_state_t debugger_state;

/*
	debugger_color_t enumeration
	This enumeration lists all the colors used by the application. Only the
	8 basic VGA colors are used (others begin unsupported most of the
	time). Some other names are defined to add a layer of semantics between
	highlighted messages and their actual color (f.i. disassembler).
*/
typedef enum
{
	/* Basic colors, initialized in debugger.c */
	color_black	= 0,
	color_red	= 1,
	color_green	= 2,
	color_yellow	= 3,
	color_blue	= 4,
	color_magenta	= 5,
	color_cyan	= 6,
	color_white	= 7,

	/* Console */
	color_command	= color_cyan,
	color_error	= color_red,
	color_idle	= color_yellow,
	color_break	= color_cyan,
	color_halt	= color_green,

	/* Disassembler */
	color_arithm	= color_white,
	color_test	= color_white,
	color_let	= color_green,
	color_jump	= color_cyan,
	color_memory	= color_red,
	color_control	= color_magenta,

} debugger_color_t;

/*
	debugger_code_init() -- compute the geometry of the assembler listing
*/
void debugger_code_init(void);

/*
	debugger_code() -- refresh the code panel, showing disassembled code
	This function either reads from the debugger's disassembler pointer or
	the emulated CPU's PC to display relevant code.
*/
void debugger_code(void);

/*
	debugger_code_mode() -- set the disassembler mode
	If follow_pc is non-zero, the disassembler follows PC during execution.
	Otherwise, it stays at address. You probably want to call
	debugger_code() after this function.

	@arg	follow_pc	First mode: always show current instruction
	@arg	address		Second mode: show fixed address
*/
void debugger_code_mode(int follow_pc, uint64_t address);

/*
	debugger_memory_move() -- display another section of the memory

	@arg	address	Top address of the new section to display
*/
void debugger_memory_move(uint64_t address);

/*
	debugger_memory() -- refresh the memory panel
*/
void debugger_memory(void);


/*
	dbglog() -- print messages to the console

	@arg	format	A printf()-like format for the message
	@arg	...	Arguments required by the format
*/
void dbglog(const char *format, ...);

/*
	dbgnlog() -- print partial messages to the console
	May be useful in some rare cases. Outputs the message using waddnstr(),
	without performing any formatting.

	@arg	message	Message to display
	@arg	n	Length of the message
*/
void dbgnlog(const char *str, size_t n);

/*
	dbgerr() -- print error messages
	This function prints 'error:' in red, then the given message by calling
	dbglog().

	@arg	format	A printf()-like format for the error message
	@args	...	Arguments required by the format
*/
void dbgerr(const char *format, ...);

/*
	debugger_prompt() -- get a command, returning a static buffer
	This function reads a command from the console window of the debugger,
	return a pointer to the text. It also provides an argument buffer which
	is guaranteed to be large enough to hold pointers all the parts of the
	command (space-separated). The argument pointer is set if non-NULL.

	@arg	args	If non-NULL, set to the address of an argument array
			suitable for splitting the command into parts
	@returns	A pointer to a static buffer (never changes)
*/
char *debugger_prompt(char ***args);

#endif	/* _DEBUGGER_SOURCE */
#endif	/* DEBUGGER_H */
